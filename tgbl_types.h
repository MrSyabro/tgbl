#ifndef TGBL_TYPES_H
#define TGBL_TYPES_H

typedef struct {
    char file_id[128];
    char title[16];
    char mime_type[16];
} tgbl_audio_t;

typedef struct {
    int id;
    char type[8];
} tgbl_chat_t;

typedef struct {
    int id;
    char is_bot;
    char username[16];
    char last_name[16];
    char first_name[16];
} tgbl_user_t;

typedef struct {
    int id;
    tgbl_user_t *from;
    tgbl_chat_t *sender_chat;
    tgbl_audio_t *audio;
    char *text;
    int date;
} tgbl_message_t;

typedef struct {
    char *token;
    int id;
    int messages_len;
    tgbl_message_t *messages;
} tgb_t;

typedef struct {
    char file_id[128];
    int file_size;
    char file_path[128];
} tgbl_file_t;

#endif