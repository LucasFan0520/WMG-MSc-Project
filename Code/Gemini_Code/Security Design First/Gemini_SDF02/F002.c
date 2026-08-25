/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

int main(void) {
    Record *collector = NULL;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *arg2 = p;

        if (strcmp(cmd, "ADD") == 0) {
            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *temp = realloc(collector, new_cap * sizeof(Record));
                if (!temp) break;
                collector = temp;
                capacity = new_cap;
            }
            collector[count].tag = strdup(arg1);
            collector[count].message = strdup(arg2);
            if (collector[count].tag && collector[count].message) {
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = arg1;
            char *tag2 = arg2;
            while (*tag2 && *tag2 != ' ') tag2++;
            if (*tag2) {
                *tag2 = '\0';
                tag2++;
            }
            while (*tag2 == ' ') tag2++;
            char *newtag = tag2;
            tag2 = arg2;

            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(collector[i].tag, tag1) == 0) {
                    m1 = collector[i].message;
                }
                if (!m2 && strcmp(collector[i].tag, tag2) == 0) {
                    m2 = collector[i].message;
                }
                if (m1 && m2) break;
            }
            if (m1 && m2) {
                size_t len1 = strlen(m1);
                size_t len2 = strlen(m2);
                char *new_msg = malloc(len1 + len2 + 2);
                if (new_msg) {
                    strcpy(new_msg, m1);
                    strcat(new_msg, " ");
                    strcat(new_msg, m2);
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        Record *temp = realloc(collector, new_cap * sizeof(Record));
                        if (!temp) {
                            free(new_msg);
                            continue;
                        }
                        collector = temp;
                        capacity = new_cap;
                    }
                    collector[count].tag = strdup(newtag);
                    collector[count].message = new_msg;
                    if (collector[count].tag) {
                        count++;
                    } else {
                        free(new_msg);
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = arg1;
            for (size_t i = 0; i < count; ) {
                if (strcmp(collector[i].tag, tag) == 0) {
                    free(collector[i].tag);
                    free(collector[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        collector[j] = collector[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = arg1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(collector[i].tag, tag) == 0) {
                    char *t = collector[i].message;
                    while (*t) {
                        if (*t == ' ') {
                            putchar('_');
                        } else {
                            putchar(*t);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", collector[i].tag, collector[i].message);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(collector[i].tag);
        free(collector[i].message);
    }
    free(collector);
    return 0;
}
