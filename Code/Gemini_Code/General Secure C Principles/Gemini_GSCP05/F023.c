// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
    int tag_capacity;
} Message;

int main(void) {
    Message *mailbox = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *subject = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *body = p + 1;
                    while (*body == ' ') body++;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Message *new_m = realloc(mailbox, capacity * sizeof(Message));
                        if (new_m) mailbox = new_m;
                    }
                    if (count < capacity) {
                        mailbox[count].sender = strdup(sender);
                        mailbox[count].subject = strdup(subject);
                        mailbox[count].body = strdup(body);
                        mailbox[count].tags = NULL;
                        mailbox[count].tag_count = 0;
                        mailbox[count].tag_capacity = 0;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *tag = p + 1;
                while (*tag == ' ') tag++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        Message *m = &mailbox[i];
                        int exists = 0;
                        for (int j = 0; j < m->tag_count; j++) {
                            if (strcmp(m->tags[j], tag) == 0) {
                                exists = 1;
                                break;
                            }
                        }
                        if (!exists) {
                            if (m->tag_count >= m->tag_capacity) {
                                m->tag_capacity = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
                                char **new_t = realloc(m->tags, m->tag_capacity * sizeof(char *));
                                if (new_t) m->tags = new_t;
                            }
                            if (m->tag_count < m->tag_capacity) {
                                m->tags[m->tag_count] = strdup(tag);
                                m->tag_count++;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *tag = p + 1;
                while (*tag == ' ') tag++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subject) == 0) {
                        Message *m = &mailbox[i];
                        for (int j = 0; j < m->tag_count; j++) {
                            if (strcmp(m->tags[j], tag) == 0) {
                                free(m->tags[j]);
                                for (int k = j; k < m->tag_count - 1; k++) {
                                    m->tags[k] = m->tags[k + 1];
                                }
                                m->tag_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            while (*subject == ' ') subject++;
            for (int i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    free(mailbox[i].sender);
                    free(mailbox[i].subject);
                    free(mailbox[i].body);
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        free(mailbox[i].tags[j]);
                    }
                    free(mailbox[i].tags);
                    for (int j = i; j < count - 1; j++) {
                        mailbox[j] = mailbox[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') tag++;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < mailbox[i].tag_count; j++) {
                    if (strcmp(mailbox[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", mailbox[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
                if (mailbox[i].tag_count > 0) {
                    printf("  Tags:");
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        printf(" %s", mailbox[i].tags[j]);
                    }
                    printf("\n");
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(mailbox[i].sender);
        free(mailbox[i].subject);
        free(mailbox[i].body);
        for (int j = 0; j < mailbox[i].tag_count; j++) {
            free(mailbox[i].tags[j]);
        }
        free(mailbox[i].tags);
    }
    free(mailbox);
    return 0;
}
