// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
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
} Inventory;

int main(void) {
    Inventory *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            char *item = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int amt = atoi(p);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    found = 1;
                    if (strcmp(cmd, "ADD") == 0) {
                        list[i].count += amt;
                    } else {
                        list[i].count -= amt;
                        if (list[i].count < 0) list[i].count = 0;
                    }
                    break;
                }
            }
            if (!found && strcmp(cmd, "ADD") == 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Inventory *nb = realloc(list, cap * sizeof(Inventory));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].item = strdup(item);
                    list[count].count = amt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, arg1) == 0) {
                    free(list[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int amt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].item, arg1) == 0) {
                    amt = list[i].count;
                    break;
                }
            }
            printf("%d\n", amt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", list[i].item, list[i].count);
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
