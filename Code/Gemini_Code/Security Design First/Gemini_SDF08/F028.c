// F028.c
#define _GNU_SOURCE
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
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *stop = p;
                char *note = space + 1;
                RouteStop *tmp = realloc(route, sizeof(RouteStop) * (count + 1));
                if (tmp) {
                    route = tmp;
                    route[count].stop = strdup(stop);
                    route[count].note = strdup(note);
                    if (route[count].stop && route[count].note) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *idx_str = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *stop = p2;
                    char *note = s2 + 1;
                    long idx = strtol(idx_str, NULL, 10);
                    if (idx >= 0 && idx <= count) {
                        RouteStop *tmp = realloc(route, sizeof(RouteStop) * (count + 1));
                        if (tmp) {
                            route = tmp;
                            for (int i = count; i > idx; i--) {
                                route[i] = route[i - 1];
                            }
                            route[idx].stop = strdup(stop);
                            route[idx].note = strdup(note);
                            if (route[idx].stop && route[idx].note) {
                                count++;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *from_str = p;
                char *to_str = space + 1;
                long fromIdx = strtol(from_str, NULL, 10);
                long toIdx = strtol(to_str, NULL, 10);
                if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count && fromIdx != toIdx) {
                    RouteStop target = route[fromIdx];
                    if (fromIdx < toIdx) {
                        for (int i = fromIdx; i < toIdx; i++) {
                            route[i] = route[i + 1];
                        }
                    } else {
                        for (int i = fromIdx; i > toIdx; i--) {
                            route[i] = route[i - 1];
                        }
                    }
                    route[toIdx] = target;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *idx_str = line + 7;
            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", route[i].stop, route[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    free(line);
    return 0;
}
