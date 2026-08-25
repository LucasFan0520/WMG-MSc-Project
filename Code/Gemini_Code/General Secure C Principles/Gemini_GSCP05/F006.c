// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *item;
    long long count;
} InvItem;

int main(void) {
    InvItem *inv = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *item_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *cnt_str = p + 1;
                long long val = strtoll(cnt_str, NULL, 10);
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item_start) == 0) {
                        inv[i].count += val;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        InvItem *new_inv = realloc(inv, capacity * sizeof(InvItem));
                        if (new_inv) inv = new_inv;
                    }
                    if (count < capacity) {
                        inv[count].item = strdup(item_start);
                        inv[count].count = val;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *item_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *cnt_str = p + 1;
                long long val = strtoll(cnt_str, NULL, 10);
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item_start) == 0) {
                        inv[i].count -= val;
                        if (inv[i].count < 0) inv[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item_start = line + 7;
            while (*item_start == ' ') item_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item_start) == 0) {
                    free(inv[i].item);
                    for (int j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item_start = line + 6;
            while (*item_start == ' ') item_start++;
            long long res = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item_start) == 0) {
                    res = inv[i].count;
                    break;
                }
            }
            printf("%lld\n", res);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %lld\n", inv[i].item, inv[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
