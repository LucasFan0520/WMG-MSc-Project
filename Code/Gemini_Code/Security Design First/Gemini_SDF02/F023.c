/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t t_count;
    size_t t_capacity;
} Message;

int main(void) {
    Message *messages = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Message *temp = realloc(messages, new_cap * sizeof(Message));
                if (!temp) break;
                messages = temp;
                capacity = new_cap;
            }
            messages[count].sender = strdup(sender);
            messages[count].subject = strdup(subject);
            messages[count].body = strdup(body);
            messages[count].tags = NULL;
            messages[count].t_count = 0;
            messages[count].t_capacity = 0;
            if (messages[count].sender && messages[count].subject && messages[count].body) {
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *tag = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    Message *m = &messages[i];
                    if (m->t_count >= m->t_capacity) {
                        size_t new_cap = m->t_capacity == 0 ? 4 : m->t_capacity * 2;
                        char **temp = realloc(m->tags, new_cap * sizeof(char *));
                        if (!temp) break;
                        m->tags = temp;
                        m->t_capacity = new_cap;
                    }
                    m->tags[m->t_count] = strdup(tag);
                    if (m->tags[m->t_count]) m->t_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *tag = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    Message *m = &messages[i];
                    for (size_t j = 0; j < m->t_count; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            free(m->tags[j]);
                            for (size_t k = j; k < m->t_count - 1; k++) {
                                m->tags[k] = m->tags[k + 1];
                            }
                            m->t_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    free(messages[i].sender);
                    free(messages[i].subject);
                    free(messages[i].body);
                    for (size_t j = 0; j < messages[i].t_count; j++) {
                        free(messages[i].tags[j]);
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
            char *tag = p;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < messages[i].t_count; j++) {
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
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", messages[i].sender, messages[i].subject, messages[i].body);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        for (size_t j = 0; j < messages[i].t_count; j++) {
            free(messages[i].tags[j]);
        }
        free(messages[i].tags);
    }
    free(messages);
    return 0;
}
