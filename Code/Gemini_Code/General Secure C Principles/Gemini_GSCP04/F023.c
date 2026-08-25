// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tag_count;
} Message;

int main(void) {
    Message *messages = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
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
            if (*sender != '\0' && *subject != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Message *new_messages = realloc(messages, capacity * sizeof(Message));
                    if (new_messages) messages = new_messages;
                }
                if (count < capacity) {
                    messages[count].sender = strdup(sender);
                    messages[count].subject = strdup(subject);
                    messages[count].body = strdup(body);
                    messages[count].tags = NULL;
                    messages[count].tag_count = 0;
                    count++;
                }
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    messages[i].tags = realloc(messages[i].tags, (messages[i].tag_count + 1) * sizeof(char *));
                    messages[i].tags[messages[i].tag_count++] = strdup(tag);
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
            while (*p == ' ' || *p == '\t') p++;
            char *subject = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
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
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                int matched = 0;
                for (size_t k = 0; k < messages[i].tag_count; k++) {
                    if (strcmp(messages[i].tags[k], tag) == 0) {
                        matched = 1;
                        break;
                    }
                }
                if (matched) {
                    printf("%s\n", messages[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s\n", messages[i].sender, messages[i].subject, messages[i].body);
            }
        }
        free(line);
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
    return 0;
}
