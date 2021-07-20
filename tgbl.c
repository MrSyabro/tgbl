#include "tgbl.h"
#include "tgbl_parser.h"
#include "tgbl_request.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "TGBL_MAIN";
static tgb_t *current_bot;

tgb_t *tgbl_init()
{
    tgb_t *bot = malloc(sizeof(tgb_t));
    if (!bot) { return bot; }

    bot->token = calloc(strlen(CONFIG_TGBT), sizeof(char));
    strcpy(bot->token, CONFIG_TGBT);
    
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

    char *response;
    response = calloc(4096, sizeof(char));

    tgbl_request (response, bot->token, "getUpdates");
    parse_getUpdates(bot, response);

    free(response);

    return 0;
}

static void tgbl_task(void* args)
{    
    while (1)
    {
        tgbl_getUpdates(current_bot);
        
        vTaskDelay(2000);
    }

    vTaskDelete(NULL);
}

void tgbl_start_updates(tgb_t *bot)
{
    current_bot = bot;
    
    xTaskCreate(tgbl_task, "tgbl_task", 4096, NULL, uxTaskPriorityGet(NULL), NULL);
}
