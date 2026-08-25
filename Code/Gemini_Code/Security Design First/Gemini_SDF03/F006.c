// F006.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    long long count;
} InvItem;

int main(void) {
    InvItem *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
            char *item = p;
            while (*p && *p != ' ') p++;
            char *cnt_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                cnt_str = p;
            }
            long long c_val = atoll(cnt_str);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    arr[i].count += c_val;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    InvItem *tmp = realloc(arr, capacity * sizeof(InvItem));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].item = strdup(item);
                arr[count].count = c_val;
                if (!arr[count].item) exit(1);
                count++;
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            char *cnt_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                cnt_str = p;
            }
            long long c_val = atoll(cnt_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    arr[i].count -= c_val;
                    if (arr[i].count < 0) arr[i].count = 0;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item = line + 7;
            while (*item == ' ') item++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    free(arr[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item = line + 6;
            while (*item == ' ') item++;
            long long c_val = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    c_val = arr[i].count;
                    break;
                }
            }
            printf("%lld\n", c_val);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", arr[i].item, arr[i].count);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].item);
    }
    free(arr);
    free(line);
    return 0;
}
