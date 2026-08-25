// F006.c
#define _GNU_SOURCE
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
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *item = args;
                int amt = atoi(space + 1);
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    inv[found].count += amt;
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        InvItem *next = realloc(inv, capacity * sizeof(InvItem));
                        if (!next) break;
                        inv = next;
                    }
                    inv[count].item = strdup(item);
                    inv[count].count = amt;
                    count++;
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *item = args;
                int amt = atoi(space + 1);
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        inv[i].count -= amt;
                        if (inv[i].count < 0) inv[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    free(inv[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item = line + 6;
            int amt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    amt = inv[i].count;
                    break;
                }
            }
            printf("%d\n", amt);
        } else if (strcmp(line, "REPORT") == 0) {
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
