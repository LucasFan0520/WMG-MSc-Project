/* F006.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InventoryItem;

int main(void) {
    InventoryItem *inv = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = args;
            int amt = atoi(space + 1);
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    inv[i].count += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                char *n_name = strdup(name);
                if (n_name) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        InventoryItem *tmp = realloc(inv, new_cap * sizeof(InventoryItem));
                        if (tmp) {
                            inv = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_name);
                            *space = ' ';
                            continue;
                        }
                    }
                    inv[count].item = n_name;
                    inv[count].count = amt;
                    count++;
                }
            }
            *space = ' ';
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = args;
            int amt = atoi(space + 1);
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    inv[i].count -= amt;
                    if (inv[i].count < 0) inv[i].count = 0;
                    break;
                }
            }
            *space = ' ';
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    free(inv[i].item);
                    for (int j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *name = line + 6;
            int found_amt = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    found_amt = inv[i].count;
                    break;
                }
            }
            printf("%d\n", found_amt);
        } else if (strcmp(line, "REPORT") == 0) {
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
