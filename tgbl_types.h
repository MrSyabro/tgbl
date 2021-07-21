#ifndef TGBL_TYPES_H
#define TGBL_TYPES_H

typedef struct {
    char *file_id;
    char *title;
    char *mime_type;
} tgbl_audio_t;

typedef struct {
    int id;
    char *type;
} tgbl_chat_t;

typedef struct {
    int id;
    char is_bot;
    char *username;
    char *last_name;
    char *first_name;
} tgbl_user_t;

typedef struct {
    int id;
    tgbl_user_t from;
    tgbl_chat_t sender_chat;
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
    char *file_id;
    int file_size;
    char *file_path;
} tgbl_file_t;

#endif