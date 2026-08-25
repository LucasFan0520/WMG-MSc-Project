// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

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

int main(void) {
    Draft *list = NULL;
    int count = 0;
    Draft last_discarded = {NULL, NULL};
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body = p;
            list = realloc(list, (count + 1) * sizeof(Draft));
            list[count].title = strdup(title);
            list[count].body = strdup(body);
            count++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body = p;
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0) {
                    free(list[i].body);
                    list[i].body = strdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0) {
                    if (last_discarded.title) free(last_discarded.title);
                    if (last_discarded.body) free(last_discarded.body);
                    last_discarded.title = list[i].title;
                    last_discarded.body = list[i].body;
                    list[i].title = NULL;
                    list[i].body = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (last_discarded.title) {
                list = realloc(list, (count + 1) * sizeof(Draft));
                list[count].title = last_discarded.title;
                list[count].body = last_discarded.body;
                count++;
                last_discarded.title = NULL;
                last_discarded.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0) {
                    printf("%s\n", list[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].title) {
                    printf("%s: %s\n", list[i].title, list[i].body);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].title) free(list[i].title);
        if (list[i].body) free(list[i].body);
    }
    free(list);
    if (last_discarded.title) free(last_discarded.title);
    if (last_discarded.body) free(last_discarded.body);
    return 0;
}
