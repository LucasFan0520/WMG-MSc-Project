// F011.c
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char *body;
} Draft;

int main() {
    Draft *items = NULL;
    int count = 0;
    int cap = 0;
    Draft last_discarded = {NULL, NULL};
    int has_discarded = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].title, items[i].body);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "RESTORE") == 0) {
            if (has_discarded) {
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    Draft *nb = realloc(items, cap * sizeof(Draft));
                    if (nb) items = nb;
                }
                items[count].title = last_discarded.title;
                items[count].body = last_discarded.body;
                count++;
                has_discarded = 0;
                last_discarded.title = NULL;
                last_discarded.body = NULL;
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Draft *nb = realloc(items, cap * sizeof(Draft));
                if (nb) items = nb;
            }
            items[count].title = strdup(title);
            items[count].body = strdup(body);
            count++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].body);
                    items[i].body = strdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    if (has_discarded) {
                        free(last_discarded.title);
                        free(last_discarded.body);
                    }
                    last_discarded.title = items[i].title;
                    last_discarded.body = items[i].body;
                    has_discarded = 1;
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("%s\n", items[i].body);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].body);
    }
    free(items);
    if (has_discarded) {
        free(last_discarded.title);
        free(last_discarded.body);
    }
    return 0;
}
