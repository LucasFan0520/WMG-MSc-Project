// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InventoryItem;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    InventoryItem *arr = NULL;
    int count_items = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                int val = atoi(p + 1);
                int found = 0;
                for (int i = 0; i < count_items; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        arr[i].count += val;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    arr = realloc(arr, (count_items + 1) * sizeof(InventoryItem));
                    arr[count_items].item = strdup(item);
                    arr[count_items].count = val;
                    count_items++;
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p = line + 5;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                int val = atoi(p + 1);
                for (int i = 0; i < count_items; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        arr[i].count -= val;
                        if (arr[i].count < 0) arr[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item = line + 7;
            for (int i = 0; i < count_items; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    free(arr[i].item);
                    for (int j = i; j < count_items - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count_items--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item = line + 6;
            int current_val = 0;
            for (int i = 0; i < count_items; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    current_val = arr[i].count;
                    break;
                }
            }
            printf("%d\n", current_val);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count_items; i++) {
                printf("%s %d\n", arr[i].item, arr[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count_items; i++) {
        free(arr[i].item);
    }
    free(arr);
    return 0;
}
