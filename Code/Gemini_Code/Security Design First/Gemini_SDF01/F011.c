// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    Draft discarded = {NULL, NULL};
    int has_discarded = 0;
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

        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *body = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "CREATE") == 0) {
                if (found != -1) {
                    free(items[found].body);
                    items[found].body = strdup(body);
                    if (!items[found].body) exit(1);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Draft *new_items = realloc(items, capacity * sizeof(Draft));
                        if (!new_items) exit(1);
                        items = new_items;
                    }
                    items[count].title = strdup(title);
                    items[count].body = strdup(body);
                    if (!items[count].title || !items[count].body) exit(1);
                    count++;
                }
            } else {
                if (found != -1) {
                    free(items[found].body);
                    items[found].body = strdup(body);
                    if (!items[found].body) exit(1);
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    if (has_discarded) {
                        free(discarded.title);
                        free(discarded.body);
                    }
                    discarded = items[i];
                    has_discarded = 1;

                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (has_discarded) {
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(items[i].title, discarded.title) == 0) {
                        found = (int)i;
                        break;
                    }
                }

                if (found != -1) {
                    free(items[found].body);
                    items[found].body = discarded.body;
                    free(discarded.title);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Draft *new_items = realloc(items, capacity * sizeof(Draft));
                        if (!new_items) exit(1);
                        items = new_items;
                    }
                    items[count] = discarded;
                    count++;
                }
                has_discarded = 0;
                discarded.title = NULL;
                discarded.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                printf("%s\n", items[found].body);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].title, items[i].body);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].body);
    }
    free(items);
    if (has_discarded) {
        free(discarded.title);
        free(discarded.body);
    }
    free(line);
    return 0;
}
