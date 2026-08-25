// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main() {
    RouteStop *route = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d: %s %s\n", i, route[i].stop, route[i].note);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char stop[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", stop, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    route = realloc(route, capacity * sizeof(RouteStop));
                }
                route[count].stop = strdup(stop);
                route[count].note = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx = 0;
            char stop[256];
            int n2 = 0;
            if (sscanf(p, "%d %255s%n", &idx, stop, &n2) == 2) {
                if (idx >= 0 && idx <= count) {
                    p += n2;
                    while (*p == ' ' || *p == '\t') p++;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        route = realloc(route, capacity * sizeof(RouteStop));
                    }
                    for (int i = count; i > idx; i--) {
                        route[i] = route[i - 1];
                    }
                    route[idx].stop = strdup(stop);
                    route[idx].note = strdup(p);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int fromIdx = 0, toIdx = 0;
            if (sscanf(p, "%d %d", &fromIdx, &toIdx) == 2) {
                if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count) {
                    RouteStop temp = route[fromIdx];
                    if (fromIdx < toIdx) {
                        for (int i = fromIdx; i < toIdx; i++) {
                            route[i] = route[i + 1];
                        }
                    } else {
                        for (int i = fromIdx; i > toIdx; i--) {
                            route[i] = route[i - 1];
                        }
                    }
                    route[toIdx] = temp;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = 0;
            if (sscanf(p, "%d", &idx) == 1) {
                if (idx >= 0 && idx < count) {
                    free(route[idx].stop);
                    free(route[idx].note);
                    for (int i = idx; i < count - 1; i++) {
                        route[i] = route[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[256];
            if (sscanf(p, "%255s", stop) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(route[i].stop, stop) == 0) {
                        printf("%d: %s\n", i, route[i].note);
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
