// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

typedef struct {
    char *item;
    int count;
} InvItem;

int main() {
    InvItem *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
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
        if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", items[i].item, items[i].count);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *cnt_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int val = atoi(cnt_str);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    items[i].count += val;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    InvItem *nb = realloc(items, cap * sizeof(InvItem));
                    if (nb) items = nb;
                }
                items[count].item = strdup(item);
                items[count].count = val;
                count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *cnt_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int val = atoi(cnt_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    items[i].count -= val;
                    if (items[i].count < 0) {
                        items[i].count = 0;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
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
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int val = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    val = items[i].count;
                    break;
                }
            }
            printf("%d\n", val);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].item);
    }
    free(items);
    return 0;
}
