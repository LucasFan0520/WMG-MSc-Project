// F028.c
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
    char *stop;
    char *note;
} RouteStop;

int main() {
    RouteStop *items = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(RouteStop));
            }
            items[count].stop = mystrdup(arg1);
            items[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx = atoi(arg1);
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(RouteStop));
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    items[i] = items[i - 1];
                }
                items[idx].stop = mystrdup(stop);
                items[idx].note = mystrdup(note);
                count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int fromIdx = atoi(arg1);
            while (*p == ' ') p++;
            char *toIdx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int toIdx = atoi(toIdx_s);
            if (fromIdx >= 0 && (size_t)fromIdx < count && toIdx >= 0 && (size_t)toIdx < count) {
                RouteStop tmp = items[fromIdx];
                if (fromIdx < toIdx) {
                    for (size_t i = (size_t)fromIdx; i < (size_t)toIdx; i++) {
                        items[i] = items[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)fromIdx; i > (size_t)toIdx; i--) {
                        items[i] = items[i - 1];
                    }
                }
                items[toIdx] = tmp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = atoi(arg1);
            if (idx >= 0 && (size_t)idx < count) {
                free(items[idx].stop);
                free(items[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].stop, arg1) == 0) {
                    printf("%s\n", items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", items[i].stop, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].stop);
        free(items[i].note);
    }
    free(items);
    return 0;
}
