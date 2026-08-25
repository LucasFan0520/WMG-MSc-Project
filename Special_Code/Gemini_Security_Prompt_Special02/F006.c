// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *item;
    int count;
} InventoryItem;

int main() {
    InventoryItem *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *item = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *cnt_s = p;
        if (strcmp(cmd, "ADD") == 0) {
            int amt = atoi(cnt_s);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    items[i].count += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(InventoryItem));
                }
                items[count].item = mystrdup(item);
                items[count].count = amt;
                count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            int amt = atoi(cnt_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    items[i].count -= amt;
                    if (items[i].count < 0) {
                        items[i].count = 0;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
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
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    printf("%d\n", items[i].count);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", items[i].item, items[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].item);
    }
    free(items);
    return 0;
}
