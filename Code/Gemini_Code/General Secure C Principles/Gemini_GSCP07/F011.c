// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Draft *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    Draft disc = {NULL, NULL};
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *nl = realloc(list, capacity * sizeof(Draft));
                    if (!nl) break;
                    list = nl;
                }
                list[count].title = strdup(title);
                list[count].body = strdup(body);
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    char *nb = strdup(body);
                    if (nb) {
                        free(list[i].body);
                        list[i].body = nb;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(disc.title);
                    free(disc.body);
                    disc.title = list[i].title;
                    disc.body = list[i].body;
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (disc.title) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *nl = realloc(list, capacity * sizeof(Draft));
                    if (!nl) break;
                    list = nl;
                }
                list[count].title = disc.title;
                list[count].body = disc.body;
                count++;
                disc.title = NULL;
                disc.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    printf("%s\n", list[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", list[i].title, list[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].body);
    }
    free(list);
    free(disc.title);
    free(disc.body);
    return 0;
}
