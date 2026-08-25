// F023.c
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
} Mail;

int main(void) {
    Mail *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *subject = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *body = p;

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Mail *new_items = realloc(items, capacity * sizeof(Mail));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].sender = strdup(sender);
            items[count].subject = strdup(subject);
            items[count].body = strdup(body);
            if (!items[count].sender || !items[count].subject || !items[count].body) exit(1);
            items[count].tags = NULL;
            items[count].tag_count = 0;
            items[count].tag_capacity = 0;
            count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].subject, subject) == 0) {
                    Mail *m = &items[i];
                    if (m->tag_count >= m->tag_capacity) {
                        m->tag_capacity = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
                        char **new_tags = realloc(m->tags, m->tag_capacity * sizeof(char *));
                        if (!new_tags) exit(1);
                        m->tags = new_tags;
                    }
                    m->tags[m->tag_count] = strdup(tag);
                    if (!m->tags[m->tag_count]) exit(1);
                    m->tag_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].subject, subject) == 0) {
                    Mail *m = &items[i];
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
            char *subject = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].subject, subject) == 0) {
                    free(items[i].sender);
                    free(items[i].subject);
                    free(items[i].body);
                    for (size_t j = 0; j < items[i].tag_count; j++) {
                        free(items[i].tags[j]);
                    }
                    free(items[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < items[i].tag_count; j++) {
                    if (strcmp(items[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", items[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s Tags:", items[i].sender, items[i].subject, items[i].body);
                for (size_t j = 0; j < items[i].tag_count; j++) {
                    printf(" %s", items[i].tags[j]);
                }
                putchar('\n');
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].sender);
        free(items[i].subject);
        free(items[i].body);
        for (size_t j = 0; j < items[i].tag_count; j++) {
            free(items[i].tags[j]);
        }
        free(items[i].tags);
    }
    free(items);
    free(line);
    return 0;
}
