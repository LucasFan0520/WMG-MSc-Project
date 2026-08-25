// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *item;
    int count;
} Inv;

int main() {
    Inv *inv = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", inv[i].item, inv[i].count);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *item = args;
                int amt = atoi(p2 + 1);
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        inv[i].count += amt;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    inv = realloc(inv, sizeof(Inv) * (count + 1));
                    inv[count].item = strdup(item);
                    inv[count].count = amt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *item = args;
                int amt = atoi(p2 + 1);
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, item) == 0) {
                        inv[i].count -= amt;
                        if (inv[i].count < 0) inv[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    free(inv[i].item);
                    for (int j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    if (count == 0) { free(inv); inv = NULL; }
                    else { inv = realloc(inv, sizeof(Inv) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = args;
            int found_count = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    found_count = inv[i].count;
                    break;
                }
            }
            printf("%d\n", found_count);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
