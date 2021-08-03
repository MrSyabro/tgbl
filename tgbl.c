#include "tgbl.h"
#include "tgbl_parser.h"
#include "tgbl_request.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

static const char *TAG = "TGBL_MAIN";
static tgb_t *current_bot;
static void (*current_callback)(tgbl_message_t *mess);

tgb_t *tgbl_init()
{
    tgb_t *bot = malloc(sizeof(tgb_t));
    if (!bot) { return bot; }

    tgbl_messages_queue = xQueueCreate( 100, sizeof(tgbl_message_t *));

    bot->token = CONFIG_TGBT;
    bot->messages = malloc(sizeof(tgbl_message_t));
    bot->last_update_id = 0;
    
    tgbl_getMe(bot);

    return bot;
}

int tgbl_getMe (tgb_t *bot)
{
    if (!bot) { return TGBS_NOINIT_ERR; }

    char *response;
    response = calloc(4096, sizeof(char));

    int e = tgbl_request(response, bot->token, "getMe");
    if (e != TGBS_OK) { free(response); return TGBS_REQUEST_ERR; }

    e = parse_getMe(bot, response);
    if (e != TGBS_OK) { free(response); return TGBS_PARSE_ERR; }

    free(response);

    return 0;
}

int tgbl_getUpdates(tgb_t *bot)
{
    if (!bot) { return TGBS_NOINIT_ERR; }

    char *response = calloc(4096, sizeof(char));
    char *request = calloc(21+16, sizeof(char));

    if (!response) { return TGBS_MALLOC_ERR; }
    if (!request) { return TGBS_MALLOC_ERR; }

    sprintf(request, "getUpdates?offset=%d", bot->last_update_id+1);

    tgbl_request (response, bot->token, request);
    parse_getUpdates(bot, response);

    free(request);
    free(response);

    return 0;
}

void tgbl_getFile(tgbl_audio_t *audio, tgbl_file_t *file)
{
    char *request = calloc(17 + strlen(audio->file_id) + 1, sizeof(char)); // strlen("getFile?file_id=")
    char *response = calloc(2048, sizeof(char));

    //if (!response) { return TGBS_MALLOC_ERR; }
    //if (!request) { return TGBS_MALLOC_ERR; }
    
    sprintf(request, "getFile?file_id=%s", audio->file_id);
    tgbl_request (response, current_bot->token, request);
    parse_getFile(file, response);

    free(response);
    free(request);
}

char *tgbl_genFileUri(tgbl_file_t *file)
{
    char *file_uri = calloc(35 + strlen(file->file_id) + 1, sizeof(char)); // strlen("https://api.telegram.org/file/bot/")
    
    sprintf(file_uri, "https://api.telegram.org/file/bot%s/%s", current_bot->token, file->file_path);

    return file_uri;
}

void tgbl_free_message_struct(tgbl_message_t *mess)
{
    if (mess->text)
    {
        free (mess->text);
    }
    if (mess->audio)
    {
        free (mess->audio->file_id);
        free (mess->audio->mime_type);
        free (mess->audio->title);
        free (mess->audio);
    }

    free(mess->from.first_name);
    free(mess->from.last_name);
    free(mess->from.username);
    free(mess->sender_chat.type);
}

static void tgbl_task(void* args)
{
    tgb_t *bot = (tgb_t *) args;
    while (1)
    {
        

        if (tgbl_getUpdates(bot) == TGBS_MALLOC_ERR) { printf("[tgbl.c] getUpdates malloc error"); }

        for (int i = 0; i < bot->messages_len; i++)
        {
            tgbl_message_t *mess = &bot->messages[i];
            xQueueSend(tgbl_messages_queue, &mess, portMAX_DELAY);
            tgbl_free_message_struct(&bot->messages[i]);
        }

        vTaskDelay(2000);
    }

    vTaskDelete(NULL);
}

void tgbl_start_updates(tgb_t *bot)
{
    xTaskCreate(tgbl_task, "tgbl_task", configMINIMAL_STACK_SIZE * 8, ( void * ) bot, uxTaskPriorityGet(NULL), NULL);
}
