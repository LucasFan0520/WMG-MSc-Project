// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_val;
    char *new_val;
} RenameRule;

int main(void) {
    RenameRule *items = NULL;
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
            char *old_val = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *new_val = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                RenameRule *new_items = realloc(items, capacity * sizeof(RenameRule));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].old_val = strdup(old_val);
            items[count].new_val = strdup(new_val);
            if (!items[count].old_val || !items[count].new_val) exit(1);
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, items[i].old_val) == 0) {
                    printf("%s\n", items[i].new_val);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_val = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].old_val, old_val) == 0) {
                    free(items[i].old_val);
                    free(items[i].new_val);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *j_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long idx_i = strtol(i_str, NULL, 10);
            long idx_j = strtol(j_str, NULL, 10);

            if (idx_i >= 0 && (size_t)idx_i < count && idx_j >= 0 && (size_t)idx_j < count) {
                RenameRule temp = items[idx_i];
                items[idx_i] = items[idx_j];
                items[idx_j] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", items[i].old_val, items[i].new_val);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].old_val);
        free(items[i].new_val);
    }
    free(items);
    free(line);
    return 0;
}
