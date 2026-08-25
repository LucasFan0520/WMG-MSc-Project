// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *item;
    int count;
} InvItem;

int main() {
    InvItem *inv = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int amt = 0;
            if (sscanf(p, "%255s %d", name, &amt) == 2) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    inv[found].count += amt;
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        inv = realloc(inv, capacity * sizeof(InvItem));
                    }
                    inv[count].item = strdup(name);
                    inv[count].count = amt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256];
            int amt = 0;
            if (sscanf(p, "%255s %d", name, &amt) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        inv[i].count -= amt;
                        if (inv[i].count < 0) {
                            inv[i].count = 0;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(inv[found].item);
                    for (int i = found; i < count - 1; i++) {
                        inv[i] = inv[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int amt = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        amt = inv[i].count;
                        break;
                    }
                }
                printf("%d\n", amt);
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
