// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main() {
    RouteStop *route = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[128];
            if (sscanf(line + 4, "%127s", name) == 1) {
                char *p = line + 4;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                route = realloc(route, (count + 1) * sizeof(RouteStop));
                route[count].stop = strdup(name);
                route[count].note = strdup(p);
                count++;
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            int idx;
            char name[128];
            if (sscanf(line + 7, "%d %127s", &idx, name) == 2) {
                char *p = line + 7;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                if (idx >= 0 && idx <= count) {
                    route = realloc(route, (count + 1) * sizeof(RouteStop));
                    for (int i = count; i > idx; i--) {
                        route[i] = route[i - 1];
                    }
                    route[idx].stop = strdup(name);
                    route[idx].note = strdup(p);
                    count++;
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            int fromIdx, toIdx;
            if (sscanf(line + 5, "%d %d", &fromIdx, &toIdx) == 2) {
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
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
                route = realloc(route, count * sizeof(RouteStop));
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, name) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", route[i].stop, route[i].note);
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
