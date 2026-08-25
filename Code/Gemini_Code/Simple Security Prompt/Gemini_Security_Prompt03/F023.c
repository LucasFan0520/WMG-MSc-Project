// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
} Mail;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Mail *mailbox = NULL;
    int mail_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            while (*p == ' ') p++;
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body = p;
            mailbox = realloc(mailbox, (mail_count + 1) * sizeof(Mail));
            mailbox[mail_count].sender = strdup(sender);
            mailbox[mail_count].subject = strdup(subject);
            mailbox[mail_count].body = strdup(body);
            mailbox[mail_count].tags = NULL;
            mailbox[mail_count].tag_count = 0;
            mail_count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < mail_count; i++) {
                if (mailbox[i].subject && strcmp(mailbox[i].subject, subject) == 0) {
                    int tc = mailbox[i].tag_count;
                    mailbox[i].tags = realloc(mailbox[i].tags, (tc + 1) * sizeof(char *));
                    mailbox[i].tags[tc] = strdup(tag);
                    mailbox[i].tag_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < mail_count; i++) {
                if (mailbox[i].subject && strcmp(mailbox[i].subject, subject) == 0) {
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        if (strcmp(mailbox[i].tags[j], tag) == 0) {
                            free(mailbox[i].tags[j]);
                            for (int k = j; k < mailbox[i].tag_count - 1; k++) {
                                mailbox[i].tags[k] = mailbox[i].tags[k + 1];
                            }
                            mailbox[i].tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < mail_count; i++) {
                if (mailbox[i].subject && strcmp(mailbox[i].subject, subject) == 0) {
                    free(mailbox[i].sender);
                    free(mailbox[i].subject);
                    free(mailbox[i].body);
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        free(mailbox[i].tags[j]);
                    }
                    free(mailbox[i].tags);
                    mailbox[i].sender = NULL;
                    mailbox[i].subject = NULL;
                    mailbox[i].body = NULL;
                    mailbox[i].tags = NULL;
                    mailbox[i].tag_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < mail_count; i++) {
                if (mailbox[i].subject) {
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        if (strcmp(mailbox[i].tags[j], tag) == 0) {
                            printf("%s\n", mailbox[i].subject);
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < mail_count; i++) {
                if (mailbox[i].subject) {
                    printf("From: %s Subject: %s Body: %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < mail_count; i++) {
        if (mailbox[i].subject) {
            free(mailbox[i].sender);
            free(mailbox[i].subject);
            free(mailbox[i].body);
            for (int j = 0; j < mailbox[i].tag_count; j++) {
                free(mailbox[i].tags[j]);
            }
            free(mailbox[i].tags);
        }
    }
    free(mailbox);
    return 0;
}
