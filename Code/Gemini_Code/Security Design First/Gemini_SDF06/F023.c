// F023.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tag_count;
    size_t tag_capacity;
} Message;

int main(void) {
    Message *messages = NULL;
    size_t msg_count = 0;
    size_t msg_capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *sender = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *body = p;

            if (*sender == '\0' || *subject == '\0') continue;

            if (msg_count >= msg_capacity) {
                size_t new_cap = msg_capacity == 0 ? 4 : msg_capacity * 2;
                Message *new_msg = realloc(messages, new_cap * sizeof(Message));
                if (!new_msg) continue;
                messages = new_msg;
                msg_capacity = new_cap;
            }
            char *snd = strdup(sender);
            char *sbj = strdup(subject);
            char *bdy = strdup(body);
            if (snd && sbj && bdy) {
                messages[msg_count].sender = snd;
                messages[msg_count].subject = sbj;
                messages[msg_count].body = bdy;
                messages[msg_count].tags = NULL;
                messages[msg_count].tag_count = 0;
                messages[msg_count].tag_capacity = 0;
                msg_count++;
            } else {
                free(snd);
                free(sbj);
                free(bdy);
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*subject == '\0' || *tag == '\0') continue;

            for (size_t i = 0; i < msg_count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    Message *m = &messages[i];
                    if (m->tag_count >= m->tag_capacity) {
                        size_t new_cap = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
                        char **new_tags = realloc(m->tags, new_cap * sizeof(char *));
                        if (!new_tags) break;
                        m->tags = new_tags;
                        m->tag_capacity = new_cap;
                    }
                    char *tg = strdup(tag);
                    if (tg) {
                        m->tags[m->tag_count] = tg;
                        m->tag_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*subject == '\0' || *tag == '\0') continue;

            for (size_t i = 0; i < msg_count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    Message *m = &messages[i];
                    for (size_t j = 0; j < m->tag_count; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            free(m->tags[j]);
                            for (size_t k = j; k < m->tag_count - 1; k++) {
                                m->tags[k] = m->tags[k + 1];
                            }
                            m->tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*subject == '\0') continue;

            for (size_t i = 0; i < msg_count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    free(messages[i].sender);
                    free(messages[i].subject);
                    free(messages[i].body);
                    for (size_t j = 0; j < messages[i].tag_count; j++) {
                        free(messages[i].tags[j]);
                    }
                    free(messages[i].tags);
                    for (size_t j = i; j < msg_count - 1; j++) {
                        messages[j] = messages[j + 1];
                    }
                    msg_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*tag == '\0') continue;

            for (size_t i = 0; i < msg_count; i++) {
                int found = 0;
                for (size_t j = 0; j < messages[i].tag_count; j++) {
                    if (strcmp(messages[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", messages[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < msg_count; i++) {
                printf("From: %s Subject: %s Body: %s Tags:", messages[i].sender, messages[i].subject, messages[i].body);
                for (size_t j = 0; j < messages[i].tag_count; j++) {
                    printf(" %s", messages[i].tags[j]);
                }
                printf("\n");
            }
        }
    }

    for (size_t i = 0; i < msg_count; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        for (size_t j = 0; j < messages[i].tag_count; j++) {
            free(messages[i].tags[j]);
        }
        free(messages[i].tags);
    }
    free(messages);
    free(line);
    return 0;
}
