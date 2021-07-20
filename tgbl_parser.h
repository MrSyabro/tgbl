#include "tgbl_types.h"

#ifndef TGBL_PARSER_H
#define TGBL_PARSER_H

int parse_getMe (tgb_t *bot, char *response);
int parse_getUpdates (tgb_t *bot, char *response);
int parse_getFile(tgbl_file_t *file, char *json);

#endif