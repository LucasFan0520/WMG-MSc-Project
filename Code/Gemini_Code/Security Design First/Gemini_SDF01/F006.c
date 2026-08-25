// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    long count;
} InvItem;

int main(void) {
    InvItem *items = NULL;
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

        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            char *item = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *cnt_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long amt = strtol(cnt_str, NULL, 10);
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "ADD") == 0) {
                if (found != -1) {
                    items[found].count += amt;
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        InvItem *new_items = realloc(items, capacity * sizeof(InvItem));
                        if (!new_items) exit(1);
                        items = new_items;
                    }
                    items[count].item = strdup(item);
                    if (!items[count].item) exit(1);
                    items[count].count = amt;
                    count++;
                }
            } else {
                if (found != -1) {
                    items[found].count -= amt;
                    if (items[found].count < 0) {
                        items[found].count = 0;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    free(items[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    cval = items[i].count;
                    break;
                }
            }
            printf("%ld\n", cval);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %ld\n", items[i].item, items[i].count);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].item);
    }
    free(items);
    free(line);
    return 0;
}
