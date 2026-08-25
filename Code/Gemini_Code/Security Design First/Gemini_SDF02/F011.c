/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *drafts = NULL;
    size_t count = 0;
    size_t capacity = 0;

    Draft last_discarded = {NULL, NULL};
    int has_discarded = 0;

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

        if (strcmp(cmd, "CREATE") == 0) {
            char *title_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body_str = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Draft *temp = realloc(drafts, new_cap * sizeof(Draft));
                if (!temp) break;
                drafts = temp;
                capacity = new_cap;
            }
            drafts[count].title = strdup(title_str);
            drafts[count].body = strdup(body_str);
            if (drafts[count].title && drafts[count].body) {
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body_str = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title_str) == 0) {
                    char *temp = strdup(body_str);
                    if (temp) {
                        free(drafts[i].body);
                        drafts[i].body = temp;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title_str) == 0) {
                    if (has_discarded) {
                        free(last_discarded.title);
                        free(last_discarded.body);
                    }
                    last_discarded.title = drafts[i].title;
                    last_discarded.body = drafts[i].body;
                    has_discarded = 1;

                    for (size_t j = i; j < count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (has_discarded) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Draft *temp = realloc(drafts, new_cap * sizeof(Draft));
                    if (!temp) break;
                    drafts = temp;
                    capacity = new_cap;
                }
                drafts[count].title = last_discarded.title;
                drafts[count].body = last_discarded.body;
                count++;
                has_discarded = 0;
                last_discarded.title = NULL;
                last_discarded.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(drafts[i].title, title_str) == 0) {
                    printf("%s\n", drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    if (has_discarded) {
        free(last_discarded.title);
        free(last_discarded.body);
    }
    return 0;
}
