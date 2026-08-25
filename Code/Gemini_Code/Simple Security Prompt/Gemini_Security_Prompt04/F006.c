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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *item;
    int count;
} Item;

int main(void) {
    Item *inv = NULL;
    int qty = 0;
    int cap = 0;
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
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                long c = strtol(p + 1, NULL, 10);
                int found = -1;
                for (int i = 0; i < qty; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    inv[found].count += c;
                } else {
                    if (qty >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        inv = realloc(inv, cap * sizeof(Item));
                    }
                    inv[qty].item = strdup(item);
                    inv[qty].count = c;
                    qty++;
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                long c = strtol(p + 1, NULL, 10);
                for (int i = 0; i < qty; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        inv[i].count -= c;
                        if (inv[i].count < 0) inv[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item = line + 7;
            while (*item == ' ') item++;
            int found = -1;
            for (int i = 0; i < qty; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(inv[found].item);
                for (int i = found; i < qty - 1; i++) {
                    inv[i] = inv[i + 1];
                }
                qty--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item = line + 6;
            while (*item == ' ') item++;
            int found = -1;
            for (int i = 0; i < qty; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%d\n", inv[found].count);
            else printf("0\n");
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < qty; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < qty; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
