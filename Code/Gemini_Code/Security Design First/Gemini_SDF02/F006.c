/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

int main(void) {
    InvItem *inv = NULL;
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
            int amt = atoi(arg2);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, arg1) == 0) {
                    inv[i].count += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    InvItem *temp = realloc(inv, new_cap * sizeof(InvItem));
                    if (!temp) break;
                    inv = temp;
                    capacity = new_cap;
                }
                inv[count].item = strdup(arg1);
                inv[count].count = amt;
                if (inv[count].item) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            int amt = atoi(arg2);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, arg1) == 0) {
                    inv[i].count -= amt;
                    if (inv[i].count < 0) {
                        inv[i].count = 0;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, arg1) == 0) {
                    free(inv[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int amt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, arg1) == 0) {
                    amt = inv[i].count;
                    break;
                }
            }
            printf("%d\n", amt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
