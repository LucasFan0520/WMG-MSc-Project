// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

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
    RouteStop *route = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                RouteStop *nr = realloc(route, capacity * sizeof(RouteStop));
                if (!nr) break;
                route = nr;
            }
            route[count].stop = strdup(stop);
            route[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && idx <= (int)count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *nr = realloc(route, capacity * sizeof(RouteStop));
                    if (!nr) break;
                    route = nr;
                }
                for (int i = (int)count; i > idx; i--) {
                    route[i] = route[i - 1];
                }
                route[idx].stop = strdup(stop);
                route[idx].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*p == ' ') p++;
            char *from_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *to_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int fromIdx = atoi(from_str);
            int toIdx = atoi(to_str);
            if (fromIdx >= 0 && fromIdx < (int)count && toIdx >= 0 && toIdx < (int)count) {
                RouteStop target = route[fromIdx];
                if (fromIdx < toIdx) {
                    for (int i = fromIdx; i < toIdx; i++) {
                        route[i] = route[i + 1];
                    }
                } else if (fromIdx > toIdx) {
                    for (int i = fromIdx; i > toIdx; i--) {
                        route[i] = route[i - 1];
                    }
                }
                route[toIdx] = target;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx = atoi(idx_str);
            if (idx >= 0 && idx < (int)count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", route[i].stop, route[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
