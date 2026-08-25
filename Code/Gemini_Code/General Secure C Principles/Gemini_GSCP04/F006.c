// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *item;
    long long count;
} InventoryItem;

int main(void) {
    InventoryItem *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *itm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *cnt_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long amt = atoll(cnt_s);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, itm) == 0) {
                    items[i].count += amt;
                    found = 1;
                    break;
                }
            }
            if (!found && *itm != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    InventoryItem *new_items = realloc(items, capacity * sizeof(InventoryItem));
                    if (new_items) items = new_items;
                }
                if (count < capacity) {
                    items[count].item = strdup(itm);
                    items[count].count = amt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *itm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *cnt_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long amt = atoll(cnt_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, itm) == 0) {
                    items[i].count -= amt;
                    if (items[i].count < 0) items[i].count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *itm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, itm) == 0) {
                    free(items[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *itm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, itm) == 0) {
                    cnt = items[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", items[i].item, items[i].count);
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
