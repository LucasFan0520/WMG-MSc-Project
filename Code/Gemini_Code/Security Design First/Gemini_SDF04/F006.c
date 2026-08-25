// F006.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

InvItem *inventory = NULL;
int count = 0;
int capacity = 0;

int find_item(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(inventory[i].item, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int c = atoi(space2 + 1);
                    int idx = find_item(arg1);
                    if (idx != -1) {
                        inventory[idx].count += c;
                    } else {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            InvItem *tmp = realloc(inventory, capacity * sizeof(InvItem));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            inventory = tmp;
                        }
                        inventory[count].item = strdup(arg1);
                        inventory[count].count = c;
                        if (inventory[count].item) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "TAKE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int c = atoi(space2 + 1);
                    int idx = find_item(arg1);
                    if (idx != -1) {
                        inventory[idx].count -= c;
                        if (inventory[idx].count < 0) {
                            inventory[idx].count = 0;
                        }
                    }
                }
            } else if (strcmp(cmd, "REMOVE") == 0) {
                int idx = find_item(arg1);
                if (idx != -1) {
                    free(inventory[idx].item);
                    for (int i = idx; i < count - 1; i++) {
                        inventory[i] = inventory[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "COUNT") == 0) {
                int idx = find_item(arg1);
                if (idx != -1) {
                    printf("%d\n", inventory[idx].count);
                } else {
                    printf("0\n");
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", inventory[i].item, inventory[i].count);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(inventory[i].item);
    }
    free(inventory);
    return 0;
}