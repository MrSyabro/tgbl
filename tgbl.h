#include <string.h>
#include <stdlib.h>
#include "tgbl_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifndef TGBL_H
#define TGBL_H

#define MAX_HTTP_OUTPUT_BUFFER 4096

typedef enum tgbl_status_e {
    TGBS_OK = 0,
    TGBS_MALLOC_ERR,
    TGBS_INIT_ERR,
    TGBS_PARSE_ERR,
    TGBS_REQUEST_ERR,
    TGBS_NOINIT_ERR,
} tgbl_status_t;

QueueHandle_t tgbl_messages_queue;

tgb_t *tgbl_init();
void tgbl_start_updates(tgb_t *bot);
int tgbl_getMe(tgb_t *bot);
int tgbl_getUpdates(tgb_t *bot);
void tgbl_getFile(tgbl_audio_t *audio, tgbl_file_t *file);
char *tgbl_genFileUri(tgbl_file_t *file);

#endif
