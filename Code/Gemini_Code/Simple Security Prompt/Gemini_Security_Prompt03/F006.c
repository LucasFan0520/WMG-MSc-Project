// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    InvItem *list = NULL;
    int total_items = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int amt = atoi(p);
            int found = 0;
            for (int i = 0; i < total_items; i++) {
                if (list[i].item && strcmp(list[i].item, item) == 0) {
                    list[i].count += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                list = realloc(list, (total_items + 1) * sizeof(InvItem));
                list[total_items].item = strdup(item);
                list[total_items].count = amt;
                total_items++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int amt = atoi(p);
            for (int i = 0; i < total_items; i++) {
                if (list[i].item && strcmp(list[i].item, item) == 0) {
                    list[i].count -= amt;
                    if (list[i].count < 0) {
                        list[i].count = 0;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < total_items; i++) {
                if (list[i].item && strcmp(list[i].item, item) == 0) {
                    free(list[i].item);
                    list[i].item = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int count = 0;
            for (int i = 0; i < total_items; i++) {
                if (list[i].item && strcmp(list[i].item, item) == 0) {
                    count = list[i].count;
                    break;
                }
            }
            printf("%d\n", count);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < total_items; i++) {
                if (list[i].item) {
                    printf("%s %d\n", list[i].item, list[i].count);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < total_items; i++) {
        if (list[i].item) free(list[i].item);
    }
    free(list);
    return 0;
}
