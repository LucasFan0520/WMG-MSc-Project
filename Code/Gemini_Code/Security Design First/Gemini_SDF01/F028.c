// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *items = NULL;
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

        if (strcmp(cmd, "ADD") == 0) {
            char *stop = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                RouteStop *new_items = realloc(items, capacity * sizeof(RouteStop));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].stop = strdup(stop);
            items[count].note = strdup(note);
            if (!items[count].stop || !items[count].note) exit(1);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *stop = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *new_items = realloc(items, capacity * sizeof(RouteStop));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    items[i] = items[i - 1];
                }
                items[idx].stop = strdup(stop);
                items[idx].note = strdup(note);
                if (!items[idx].stop || !items[idx].note) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *to_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long from_idx = strtol(from_str, NULL, 10);
            long to_idx = strtol(to_str, NULL, 10);

            if (from_idx >= 0 && (size_t)from_idx < count && to_idx >= 0 && (size_t)to_idx < count) {
                RouteStop target = items[from_idx];
                if (from_idx < to_idx) {
                    for (size_t i = (size_t)from_idx; i < (size_t)to_idx; i++) {
                        items[i] = items[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)from_idx; i > (size_t)to_idx; i--) {
                        items[i] = items[i - 1];
                    }
                }
                items[to_idx] = target;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx < count) {
                free(items[idx].stop);
                free(items[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].stop, stop) == 0) {
                    printf("%s\n", items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s - %s\n", items[i].stop, items[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].stop);
        free(items[i].note);
    }
    free(items);
    free(line);
    return 0;
}
