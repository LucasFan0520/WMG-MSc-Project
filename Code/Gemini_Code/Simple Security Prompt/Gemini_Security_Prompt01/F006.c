// F006.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InventoryItem;

int main() {
    InventoryItem *inv = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *item = args;
            char *space2 = strchr(args, ' ');
            int amt = 0;
            if (space2) {
                *space2 = '\0';
                amt = atoi(space2 + 1);
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    inv[i].count += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                inv = realloc(inv, (count + 1) * sizeof(InventoryItem));
                if (!inv) return 1;
                inv[count].item = strdup(item);
                inv[count].count = amt;
                count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0 && args) {
            char *item = args;
            char *space2 = strchr(args, ' ');
            int amt = 0;
            if (space2) {
                *space2 = '\0';
                amt = atoi(space2 + 1);
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    inv[i].count -= amt;
                    if (inv[i].count < 0) {
                        inv[i].count = 0;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, args) == 0) {
                    free(inv[i].item);
                    for (int j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    inv = realloc(inv, count * sizeof(InventoryItem));
                    if (count > 0 && !inv) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0 && args) {
            int current_count = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, args) == 0) {
                    current_count = inv[i].count;
                    break;
                }
            }
            printf("%d\n", current_count);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
