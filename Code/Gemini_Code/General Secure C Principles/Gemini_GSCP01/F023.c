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
} MailMessage;

int main(void) {
    MailMessage *messages = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *sender = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *body = p;
            MailMessage *temp = realloc(messages, (count + 1) * sizeof(MailMessage));
            if (temp) {
                messages = temp;
                messages[count].sender = strdup(sender);
                messages[count].subject = strdup(subject);
                messages[count].body = strdup(body);
                messages[count].tags = NULL;
                messages[count].tag_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    char **temp = realloc(messages[i].tags, (messages[i].tag_count + 1) * sizeof(char *));
                    if (temp) {
                        messages[i].tags = temp;
                        messages[i].tags[messages[i].tag_count] = strdup(tag);
                        messages[i].tag_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    for (size_t k = 0; k < messages[i].tag_count; k++) {
                        if (strcmp(messages[i].tags[k], tag) == 0) {
                            free(messages[i].tags[k]);
                            for (size_t j = k; j < messages[i].tag_count - 1; j++) {
                                messages[i].tags[j] = messages[i].tags[j + 1];
                            }
                            messages[i].tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    free(messages[i].sender);
                    free(messages[i].subject);
                    free(messages[i].body);
                    for (size_t k = 0; k < messages[i].tag_count; k++) {
                        free(messages[i].tags[k]);
                    }
                    free(messages[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        messages[j] = messages[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t k = 0; k < messages[i].tag_count; k++) {
                    if (strcmp(messages[i].tags[k], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", messages[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s\n", messages[i].sender, messages[i].subject, messages[i].body);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        for (size_t k = 0; k < messages[i].tag_count; k++) {
            free(messages[i].tags[k]);
        }
        free(messages[i].tags);
    }
    free(messages);
    free(line);
    return 0;
}
