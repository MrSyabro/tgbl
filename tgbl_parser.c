#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "tgbl_parser.h"
#include "jsmn.h"


static inline void *realloc_it(void *ptrmem, size_t size) {
  void *p = realloc(ptrmem, size);
  if (!p) {
    free(ptrmem);
    fprintf(stderr, "realloc(): errno=%d\n", errno);
  }
  return p;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

static int parse (char *json, size_t len, jsmntok_t **et)
{
    size_t tcount = 16;
    jsmn_parser p;
    jsmntok_t *t;
    t = calloc(tcount, sizeof(jsmntok_t));
    if (!t) { return 1; }

    jsmn_init(&p);
    int r = jsmn_parse(&p, json, len, t, tcount);
    while (r == JSMN_ERROR_NOMEM){
        tcount = tcount * 2;
        t = realloc_it(t, sizeof(*t) * tcount);
        r = jsmn_parse(&p, json, len, t, tcount);
    }
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }

    *et = t;
    return r;
}

static int parse_audio(tgbl_audio_t *audio, char *json, jsmntok_t *et)
{
    jsmntok_t *t;
    char *str = json + et->start;
    int r = parse(str, et->end - et->start, &t);

    for (int i = 1; i < r; i++)
    {
        jsmntok_t *ct = &t[i];
        char *debug = str + ct->start;
        if (jsoneq(str, &t[i], "file_id") == 0)
        {
            strncpy(audio->file_id, str + t[i + 1].start, t[i + 1].end - t[i + 1].start);
            printf("- ID: %s\n", audio->file_id);
            i++;
        }else if (jsoneq(str, &t[i], "mime_type") == 0)
        {
            strncpy(audio->mime_type, str + t[i + 1].start, t[i + 1].end - t[i + 1].start);
            printf("- MIME type: %s\n", audio->mime_type);
            i++;
        }else if (jsoneq(str, &t[i], "title") == 0)
        {
            strncpy(audio->title, str + t[i + 1].start, t[i + 1].end - t[i + 1].start);
            printf("- Title: %s\n", audio->title);
            i++;
        }
    }

    return r;
}

static int parse_message (tgbl_message_t *message, char *json, jsmntok_t *et)
{
    jsmntok_t *t;
    char *mess_str = json + et->start;
    int r = parse(mess_str, et->end - et->start, &t);

    for (int i = 1; i < r; i++)
    {
        jsmntok_t *ct = &t[i];
        char *debug = mess_str + ct->start;
        if (jsoneq(mess_str, &t[i], "message_id") == 0)
        {
            char *id_str = calloc(t[i + 1].end - t[i + 1].start, sizeof(char));
            strncpy(id_str, mess_str + t[i + 1].start, t[i + 1].end - t[i + 1].start);
            message->id = atoi(id_str);
            free (id_str);
            printf("- ID: %d\n", message->id);
            i++;
        }else if (jsoneq(mess_str, &t[i], "text") == 0)
        {
            message->text = calloc (t[i + 1].end - t[i + 1].start, sizeof(char));
            strncpy(message->text, mess_str + t[i + 1].start, t[i + 1].end - t[i + 1].start);
            printf("- Text: %s\n", message->text);
            i++;
        }else if (jsoneq(mess_str, &t[i], "audio") == 0)
        {
            printf("* Audio: \n");
            message->audio = malloc(sizeof(tgbl_audio_t));
            i = i + parse_audio(message->audio, mess_str, &t[i + 1]);
        }
    }

    return r;
}

// TODO:
static void parse_file(tgbl_file_t *file, char *json, jsmntok_t *t)
{
    
}

static int get_result(char *json, char **result, size_t *res_str_len)
{
    int r;
    int i;
    int h;
    jsmntok_t *t;

    r = parse (json, strlen(json), &t);

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
    printf("Object expected\n");
    return 1;
    }

    for (i = 1; i < r; i++) {
        if (jsoneq(json, &t[i], "result") == 0) {
            /* We may use strndup() to fetch string value */
            printf("* Result: \n  Size: %d\n", t[i+1].type, t[i+1].size);
            
            //*result = calloc(t[i + 1].end - t[i + 1].start, sizeof(char));
            //strncpy(*result, json+t[i + 1].start, t[i + 1].end - t[i + 1].start);
            *result = json+t[i + 1].start;
            *res_str_len = t[i + 1].end - t[i + 1].start;
            return 0;
        } 
    }

    return 1;
}

int parse_getMe (tgb_t *bot, char *json)
{
    int i;
    int r;
    jsmntok_t *t; /* We expect no more than 128 tokens */

    r = parse(json, strlen(json), &t);

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
    printf("Object expected\n");
    return 1;
    }

    for (i = 0; i < r; i++)
    {
        if (jsoneq(json, &t[i], "id") == 0)
        {
            printf("- ID: %.*s\n", t[i + 1].end - t[i + 1].start,
                    json + t[i + 1].start);
            size_t l = t[i + 1].end - t[i + 1].start;
            char *id_s = calloc(10, sizeof(char));
            strncpy(id_s, json + t[i + 1].start, l);

            bot->id = atoi(id_s);
            free(id_s);
            i++;
        }
    }

    return 0;
}

int parse_getUpdates (tgb_t *bot, char *json)
{
    int i, k;
    int m = 0;
    char *result_str;
    size_t res_str_len;
    jsmntok_t *result;

    int e = get_result(json, &result_str, &res_str_len);
    //free (json);
    json = result_str;
    if (e != 0) { return 1; }
    int r = parse(json, res_str_len, &result);

    bot->messages_len = result[0].size;
    free(bot->messages);
    bot->messages = calloc(result[0].size, sizeof(tgbl_message_t));

    for (i = 1; i < r; i++) // Обходим updates массив
    {
        jsmntok_t *t = &result[i]; // temp
        if (jsoneq(json, &result[i], "message") == 0)
        {
            printf("* Message:\n");
            i = i + parse_message(&bot->messages[m], json, &result[i + 1]);
            m++;
        }
    }
    return 0;
}