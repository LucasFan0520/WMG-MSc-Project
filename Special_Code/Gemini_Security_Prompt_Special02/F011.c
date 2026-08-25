// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main() {
    Draft *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    char *last_discarded_title = NULL;
    char *last_discarded_body = NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *title = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *body = p;
        if (strcmp(cmd, "CREATE") == 0) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(Draft));
            }
            items[count].title = mystrdup(title);
            items[count].body = mystrdup(body);
            count++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].body);
                    items[i].body = mystrdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(last_discarded_title);
                    free(last_discarded_body);
                    last_discarded_title = mystrdup(items[i].title);
                    last_discarded_body = mystrdup(items[i].body);
                    free(items[i].title);
                    free(items[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (last_discarded_title) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(Draft));
                }
                items[count].title = mystrdup(last_discarded_title);
                items[count].body = mystrdup(last_discarded_body);
                count++;
                free(last_discarded_title);
                free(last_discarded_body);
                last_discarded_title = NULL;
                last_discarded_body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("%s %s\n", items[i].title, items[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].title, items[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].body);
    }
    free(items);
    free(last_discarded_title);
    free(last_discarded_body);
    return 0;
}
