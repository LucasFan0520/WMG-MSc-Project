// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int cnt;
} InventoryItem;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    InventoryItem *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *cnt_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int amt = atoi(cnt_str);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    list[i].cnt += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    InventoryItem *nl = realloc(list, capacity * sizeof(InventoryItem));
                    if (!nl) break;
                    list = nl;
                }
                list[count].item = strdup(item);
                list[count].cnt = amt;
                count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *cnt_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int amt = atoi(cnt_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    list[i].cnt -= amt;
                    if (list[i].cnt < 0) list[i].cnt = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    free(list[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    cval = list[i].cnt;
                    break;
                }
            }
            printf("%d\n", cval);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", list[i].item, list[i].cnt);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].item);
    }
    free(list);
    return 0;
}
