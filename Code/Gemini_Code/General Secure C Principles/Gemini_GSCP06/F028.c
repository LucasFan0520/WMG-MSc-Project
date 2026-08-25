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
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *stop = args;
                char *note = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *next = realloc(route, capacity * sizeof(RouteStop));
                    if (!next) break;
                    route = next;
                }
                route[count].stop = strdup(stop);
                route[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *args = line + 7;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                int idx = atoi(args);
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2 && idx >= 0 && (size_t)idx <= count) {
                    *sp2 = '\0';
                    char *stop = rest;
                    char *note = sp2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        RouteStop *next = realloc(route, capacity * sizeof(RouteStop));
                        if (!next) break;
                        route = next;
                    }
                    for (size_t j = count; j > (size_t)idx; j--) {
                        route[j] = route[j - 1];
                    }
                    route[idx].stop = strdup(stop);
                    route[idx].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                int fromIdx = atoi(args);
                int toIdx = atoi(space + 1);
                if (fromIdx >= 0 && (size_t)fromIdx < count && toIdx >= 0 && (size_t)toIdx < count && fromIdx != toIdx) {
                    RouteStop temp = route[fromIdx];
                    if (fromIdx < toIdx) {
                        for (size_t j = (size_t)fromIdx; j < (size_t)toIdx; j++) {
                            route[j] = route[j + 1];
                        }
                    } else {
                        for (size_t j = (size_t)fromIdx; j > (size_t)toIdx; j--) {
                            route[j] = route[j - 1];
                        }
                    }
                    route[toIdx] = temp;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && (size_t)idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (size_t j = (size_t)idx; j < count - 1; j++) {
                    route[j] = route[j + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", route[i].stop, route[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    free(line);
    return 0;
}
