// F006.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InventoryItem;

int main(void) {
    InventoryItem *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *item = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *cnt_s = p;
            int add_cnt = atoi(cnt_s);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    list[i].count += add_cnt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                InventoryItem *temp = realloc(list, (count + 1) * sizeof(InventoryItem));
                if (temp) {
                    list = temp;
                    list[count].item = strdup(item);
                    list[count].count = add_cnt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *item = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *cnt_s = p;
            int take_cnt = atoi(cnt_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    list[i].count -= take_cnt;
                    if (list[i].count < 0) {
                        list[i].count = 0;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *item = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
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
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *item = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    cnt = list[i].count;
                    break;
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", list[i].item, list[i].count);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].item);
    }
    free(list);
    free(line);
    return 0;
}
