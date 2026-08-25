// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *route = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char stop[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", stop) == 1) {
                char *note = strchr(p, ' ');
                if (note) {
                    while (*note == ' ') note++;
                }
                if (!note) note = "";
                RouteStop *tmp = realloc(route, (count + 1) * sizeof(RouteStop));
                if (tmp) {
                    route = tmp;
                    route[count].stop = strdup(stop);
                    route[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            int idx;
            char stop[256];
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %255s", &idx, stop) == 2) {
                if (idx >= 0 && idx <= count) {
                    char *note = strchr(p, ' ');
                    if (note) {
                        note = strchr(note + 1, ' ');
                        if (note) {
                            while (*note == ' ') note++;
                        }
                    }
                    if (!note) note = "";
                    RouteStop *tmp = realloc(route, (count + 1) * sizeof(RouteStop));
                    if (tmp) {
                        route = tmp;
                        for (int j = count; j > idx; j--) {
                            route[j] = route[j - 1];
                        }
                        route[idx].stop = strdup(stop);
                        route[idx].note = strdup(note);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            int from, to;
            if (sscanf(line + 5, "%d %d", &from, &to) == 2) {
                if (from >= 0 && from < count && to >= 0 && to < count) {
                    RouteStop target = route[from];
                    if (from < to) {
                        for (int j = from; j < to; j++) {
                            route[j] = route[j + 1];
                        }
                    } else {
                        for (int j = from; j > to; j--) {
                            route[j] = route[j - 1];
                        }
                    }
                    route[to] = target;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            int idx;
            if (sscanf(line + 7, "%d", &idx) == 1) {
                if (idx >= 0 && idx < count) {
                    free(route[idx].stop);
                    free(route[idx].note);
                    for (int j = idx; j < count - 1; j++) {
                        route[j] = route[j + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            while (*stop == ' ') stop++;
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("[%d] %s: %s\n", i, route[i].stop, route[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
