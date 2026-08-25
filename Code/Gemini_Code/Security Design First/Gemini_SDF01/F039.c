// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char **cells;
    long rows;
    long cols;
} CountedTable;

int main(void) {
    CountedTable *items = NULL;
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

        if (strcmp(cmd, "TABLE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *r_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *c_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long rows = strtol(r_str, NULL, 10);
            long cols = strtol(c_str, NULL, 10);

            if (rows > 0 && cols > 0 && rows < 10000 && cols < 10000) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    CountedTable *new_items = realloc(items, capacity * sizeof(CountedTable));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                items[count].cells = calloc(rows * cols, sizeof(char *));
                items[count].rows = rows;
                items[count].cols = cols;
                if (!items[count].name || !items[count].cells) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *r_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *c_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *value = p;

            long r = strtol(r_str, NULL, 10);
            long c = strtol(c_str, NULL, 10);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (r >= 0 && r < items[i].rows && c >= 0 && c < items[i].cols) {
                        long idx = r * items[i].cols + c;
                        free(items[i].cells[idx]);
                        items[i].cells[idx] = strdup(value);
                        if (!items[i].cells[idx]) exit(1);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *r_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *c_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long r = strtol(r_str, NULL, 10);
            long c = strtol(c_str, NULL, 10);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (r >= 0 && r < items[i].rows && c >= 0 && c < items[i].cols) {
                        long idx = r * items[i].cols + c;
                        if (items[i].cells[idx]) {
                            printf("%s\n", items[i].cells[idx]);
                        } else {
                            printf("EMPTY\n");
                        }
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    long total = items[i].rows * items[i].cols;
                    for (long j = 0; j < total; j++) {
                        free(items[i].cells[j]);
                    }
                    free(items[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        long total = items[i].rows * items[i].cols;
        for (long j = 0; j < total; j++) {
            free(items[i].cells[j]);
        }
        free(items[i].cells);
    }
    free(items);
    free(line);
    return 0;
}
