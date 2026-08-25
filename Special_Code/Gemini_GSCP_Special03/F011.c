// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    Draft undone = {NULL, NULL};
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *body = p;
            size_t blen = strlen(body);
            while (blen > 0 && (body[blen-1] == '\n' || body[blen-1] == '\n')) {
                body[blen-1] = '\0';
                blen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Draft *nitems = realloc(items, cap * sizeof(Draft));
                if (nitems) items = nitems;
            }
            items[count].title = mystrdup(title);
            items[count].body = mystrdup(body);
            count++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *body = p;
            size_t blen = strlen(body);
            while (blen > 0 && (body[blen-1] == '\n' || body[blen-1] == '\n')) {
                body[blen-1] = '\0';
                blen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].body);
                    items[i].body = mystrdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(undone.title);
                    free(undone.body);
                    undone.title = items[i].title;
                    undone.body = items[i].body;
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (undone.title) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Draft *nitems = realloc(items, cap * sizeof(Draft));
                    if (nitems) items = nitems;
                }
                items[count].title = undone.title;
                items[count].body = undone.body;
                count++;
                undone.title = NULL;
                undone.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("%s\n", items[i].body);
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
    free(undone.title);
    free(undone.body);
    return 0;
}
