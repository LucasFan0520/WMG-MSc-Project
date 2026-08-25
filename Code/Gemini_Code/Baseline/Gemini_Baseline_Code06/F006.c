// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *item;
    int count;
} InventoryItem;

int main() {
    InventoryItem *inv = NULL;
    int total = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[128];
            int qty;
            if (sscanf(line + 4, "%127s %d", name, &qty) == 2) {
                int found = 0;
                for (int i = 0; i < total; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        inv[i].count += qty;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    inv = realloc(inv, (total + 1) * sizeof(InventoryItem));
                    inv[total].item = strdup(name);
                    inv[total].count = qty;
                    total++;
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char name[128];
            int qty;
            if (sscanf(line + 5, "%127s %d", name, &qty) == 2) {
                for (int i = 0; i < total; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        inv[i].count -= qty;
                        if (inv[i].count < 0) inv[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < total; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    free(inv[i].item);
                    for (int j = i; j < total - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    total--;
                    inv = realloc(inv, total * sizeof(InventoryItem));
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *name = line + 6;
            int found = 0;
            for (int i = 0; i < total; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    printf("%d\n", inv[i].count);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("0\n");
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < total; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < total; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
