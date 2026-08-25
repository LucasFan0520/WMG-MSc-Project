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
} MailMessage;

int main(void) {
    MailMessage *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *sender = p;
            while (*p && *p != ' ') p++;
            char *subject = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                subject = p;
            }
            while (*p && *p != ' ') p++;
            char *body = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                body = p;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                MailMessage *tmp = realloc(arr, capacity * sizeof(MailMessage));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].sender = strdup(sender);
            arr[count].subject = strdup(subject);
            arr[count].body = strdup(body);
            arr[count].tags = NULL;
            arr[count].tag_count = 0;
            arr[count].tag_capacity = 0;
            if (!arr[count].sender || !arr[count].subject || !arr[count].body) exit(1);
            count++;
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            char *tag = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                tag = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subject) == 0) {
                    MailMessage *m = &arr[i];
                    if (m->tag_count >= m->tag_capacity) {
                        m->tag_capacity = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
                        char **tmp = realloc(m->tags, m->tag_capacity * sizeof(char *));
                        if (!tmp) exit(1);
                        m->tags = tmp;
                    }
                    m->tags[m->tag_count] = strdup(tag);
                    if (!m->tags[m->tag_count]) exit(1);
                    m->tag_count++;
                    break;
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            char *tag = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                tag = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subject) == 0) {
                    MailMessage *m = &arr[i];
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            while (*subject == ' ') subject++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subject) == 0) {
                    free(arr[i].sender);
                    free(arr[i].subject);
                    free(arr[i].body);
                    for (size_t j = 0; j < arr[i].tag_count; j++) {
                        free(arr[i].tags[j]);
                    }
                    free(arr[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') tag++;
            for (size_t i = 0; i < count; i++) {
                int t_found = 0;
                for (size_t j = 0; j < arr[i].tag_count; j++) {
                    if (strcmp(arr[i].tags[j], tag) == 0) {
                        t_found = 1;
                        break;
                    }
                }
                if (t_found) {
                    printf("%s\n", arr[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Sender: %s, Subject: %s, Body: %s\n", arr[i].sender, arr[i].subject, arr[i].body);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].sender);
        free(arr[i].subject);
        free(arr[i].body);
        for (size_t j = 0; j < arr[i].tag_count; j++) {
            free(arr[i].tags[j]);
        }
        free(arr[i].tags);
    }
    free(arr);
    free(line);
    return 0;
}
