// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *route = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            if (*stop != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *new_route = realloc(route, capacity * sizeof(RouteStop));
                    if (new_route) route = new_route;
                }
                if (count < capacity) {
                    route[count].stop = strdup(stop);
                    route[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            long long idx = atoll(idx_s);
            if (idx >= 0 && (size_t)idx <= count && *stop != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *new_route = realloc(route, capacity * sizeof(RouteStop));
                    if (new_route) route = new_route;
                }
                if (count < capacity) {
                    for (size_t i = count; i > (size_t)idx; i--) {
                        route[i] = route[i - 1];
                    }
                    route[idx].stop = strdup(stop);
                    route[idx].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *from_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *to_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long from_i = atoll(from_s);
            long long to_i = atoll(to_s);
            if (from_i >= 0 && (size_t)from_i < count && to_i >= 0 && (size_t)to_i < count && from_i != to_i) {
                RouteStop tmp = route[from_i];
                if (from_i < to_i) {
                    for (size_t i = (size_t)from_i; i < (size_t)to_i; i++) {
                        route[i] = route[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)from_i; i > (size_t)to_i; i--) {
                        route[i] = route[i - 1];
                    }
                }
                route[to_i] = tmp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long idx = atoll(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%zu] %s: %s\n", i, route[i].stop, route[i].note);
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
