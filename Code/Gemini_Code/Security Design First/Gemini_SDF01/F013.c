// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main(void) {
    Task *items = NULL;
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

        if (strcmp(cmd, "TODO") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                free(items[found].note);
                items[found].note = strdup(note);
                if (!items[found].note) exit(1);
                items[found].status = 0;
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Task *new_items = realloc(items, capacity * sizeof(Task));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].title = strdup(title);
                items[count].note = strdup(note);
                if (!items[count].title || !items[count].note) exit(1);
                items[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    items[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    items[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    const char *st_str = (items[i].status == 0) ? "TODO" : ((items[i].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st_str, items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t s = 0; s < 3; s++) {
                for (size_t i = 0; i < count; i++) {
                    if (items[i].status == (int)s) {
                        const char *st_str = (s == 0) ? "TODO" : ((s == 1) ? "DOING" : "DONE");
                        printf("%s: %s %s\n", st_str, items[i].title, items[i].note);
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].note);
    }
    free(items);
    free(line);
    return 0;
}
