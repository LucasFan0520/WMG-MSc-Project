// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *item;
    int count;
} InventoryItem;

int main(void) {
    InventoryItem *inv = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *item = p;
            while (*item == ' ') item++;
            char *item_end = item;
            while (*item_end && *item_end != ' ') item_end++;
            char *cnt_s = item_end;
            if (*item_end != '\0') {
                *item_end = '\0';
                cnt_s++;
            }
            while (*cnt_s == ' ') cnt_s++;
            int amt = atoi(cnt_s);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    inv[i].count += amt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    InventoryItem *new_inv = realloc(inv, capacity * sizeof(InventoryItem));
                    if (!new_inv) {
                        free(line);
                        break;
                    }
                    inv = new_inv;
                }
                inv[count].item = mystrdup(item);
                inv[count].count = amt;
                count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = p;
            while (*item == ' ') item++;
            char *item_end = item;
            while (*item_end && *item_end != ' ') item_end++;
            char *cnt_s = item_end;
            if (*item_end != '\0') {
                *item_end = '\0';
                cnt_s++;
            }
            while (*cnt_s == ' ') cnt_s++;
            int amt = atoi(cnt_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    inv[i].count -= amt;
                    if (inv[i].count < 0) inv[i].count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = p;
            while (*item == ' ') item++;
            char *item_end = item;
            while (*item_end && *item_end != ' ') item_end++;
            *item_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    free(inv[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = p;
            while (*item == ' ') item++;
            char *item_end = item;
            while (*item_end && *item_end != ' ') item_end++;
            *item_end = '\0';
            int amt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    amt = inv[i].count;
                    break;
                }
            }
            printf("%d\n", amt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
