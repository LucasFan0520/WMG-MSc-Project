// F011.c
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
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    Draft discarded = {NULL, NULL};
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
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            char *title = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    found = 1;
                    if (strcmp(cmd, "REVISE") == 0) {
                        free(list[i].body);
                        list[i].body = strdup(body);
                    }
                    break;
                }
            }
            if (!found && strcmp(cmd, "CREATE") == 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Draft *nb = realloc(list, cap * sizeof(Draft));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].title = strdup(title);
                    list[count].body = strdup(body);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    free(discarded.title);
                    free(discarded.body);
                    discarded.title = list[i].title;
                    discarded.body = list[i].body;
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discarded.title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Draft *nb = realloc(list, cap * sizeof(Draft));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].title = discarded.title;
                    list[count].body = discarded.body;
                    count++;
                    discarded.title = NULL;
                    discarded.body = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    printf("%s %s\n", list[i].title, list[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].title, list[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].body);
    }
    free(list);
    free(discarded.title);
    free(discarded.body);
    return 0;
}
