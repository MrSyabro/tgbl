#include <string.h>
#include <stdlib.h>
#include "tgbl_types.h"

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

tgb_t *tgbl_init();
void tgbl_start_updates(tgb_t *bot);
int tgbl_getMe(tgb_t *bot);
int tgbl_getUpdates(tgb_t *bot);

#endif
