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
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *item_start = p;
            char *space = strchr(item_start, ' ');
            if (!space) continue;
            *space = '\0';
            char *c_str = space + 1;
            while (*c_str == ' ') c_str++;
            int amt = atoi(c_str);
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                inv[found].count += amt;
            } else {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    InvItem *tmp = realloc(inv, new_cap * sizeof(InvItem));
                    if (!tmp) {
                        free(line);
                        for(int i=0; i<count; i++) free(inv[i].item);
                        free(inv);
                        return 1;
                    }
                    inv = tmp;
                    capacity = new_cap;
                }
                char *ni = strdup(item_start);
                if (!ni) {
                    free(line);
                    for(int i=0; i<count; i++) free(inv[i].item);
                    free(inv);
                    return 1;
                }
                inv[count].item = ni;
                inv[count].count = amt;
                count++;
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *item_start = p;
            char *space = strchr(item_start, ' ');
            if (!space) continue;
            *space = '\0';
            char *c_str = space + 1;
            while (*c_str == ' ') c_str++;
            int amt = atoi(c_str);
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                inv[found].count -= amt;
                if (inv[found].count < 0) inv[found].count = 0;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item_start = line + 7;
            while (*item_start == ' ') item_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(inv[found].item);
                for (int i = found; i < count - 1; i++) {
                    inv[i] = inv[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item_start = line + 6;
            while (*item_start == ' ') item_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%d\n", inv[found].count);
            } else {
                printf("0\n");
            }
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
