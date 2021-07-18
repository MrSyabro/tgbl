#include "tgbl.h"

static const char *TAG = "TGBL_MAIN";
static tgb_t *current_bot;

int tgbl_init(tgb_t *bot, char *tgbt)
{
    bot = malloc(sizeof(tgb_t));
    if (!bot) { return TGBS_MALLOC_ERR; }

    bot->token = tgbt;
    
    int e = tgbl_getMe(bot);
    if (e != 0) { return TGBS_INIT_ERR; }
    
    return 0;
}

int tgbl_getMe (tgb_t *bot)
{
    if (!bot) { return TGBS_NOINIT_ERR; }

    char *response;
    response = calloc(4096, sizeof(char));

    int e = tgbl_request (response, "getUpdates");
    if (e != TGBS_OK) { free(response); return TGBS_REQUEST_ERR; }

    e = parse_updates(bot, response);
    if (e != TGBS_OK) { free(response); return TGBS_PARSE_ERR; }

    free(response);

    return 0;
}

int tgbl_getUpdates(tgb_t *bot)
{
    if (!bot) { return TGBS_NOINIT_ERR; }

    char *response;
    response = calloc(4096, sizeof(char));

    tgbl_request (response, "getUpdates");
    parse_updates(bot, response);

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
