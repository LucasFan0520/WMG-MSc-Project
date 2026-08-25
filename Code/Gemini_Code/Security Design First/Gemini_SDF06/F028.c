// F028.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop_name;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *stops = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
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

            if (*stop == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                RouteStop *new_stops = realloc(stops, new_cap * sizeof(RouteStop));
                if (!new_stops) continue;
                stops = new_stops;
                capacity = new_cap;
            }
            char *s = strdup(stop);
            char *n = strdup(note);
            if (s && n) {
                stops[count].stop_name = s;
                stops[count].note = n;
                count++;
            } else {
                free(s);
                free(n);
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_str = p;
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

            if (*idx_str == '\0' || *stop == '\0') continue;
            long index = strtol(idx_str, NULL, 10);
            if (index < 0 || (size_t)index > count) continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                RouteStop *new_stops = realloc(stops, new_cap * sizeof(RouteStop));
                if (!new_stops) continue;
                stops = new_stops;
                capacity = new_cap;
            }
            char *s = strdup(stop);
            char *n = strdup(note);
            if (s && n) {
                for (size_t i = count; i > (size_t)index; i--) {
                    stops[i] = stops[i - 1];
                }
                stops[index].stop_name = s;
                stops[index].note = n;
                count++;
            } else {
                free(s);
                free(n);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *from_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *to_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*from_str == '\0' || *to_str == '\0') continue;
            long from_idx = strtol(from_str, NULL, 10);
            long to_idx = strtol(to_str, NULL, 10);
            if (from_idx < 0 || (size_t)from_idx >= count || to_idx < 0 || (size_t)to_idx >= count) continue;

            if (from_idx != to_idx) {
                RouteStop target = stops[from_idx];
                if (from_idx < to_idx) {
                    for (size_t i = (size_t)from_idx; i < (size_t)to_idx; i++) {
                        stops[i] = stops[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)from_idx; i > (size_t)to_idx; i--) {
                        stops[i] = stops[i - 1];
                    }
                }
                stops[to_idx] = target;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*idx_str == '\0') continue;
            long index = strtol(idx_str, NULL, 10);
            if (index < 0 || (size_t)index >= count) continue;

            free(stops[index].stop_name);
            free(stops[index].note);
            for (size_t i = (size_t)index; i < count - 1; i++) {
                stops[i] = stops[i + 1];
            }
            count--;
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*stop == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(stops[i].stop_name, stop) == 0) {
                    printf("%s\n", stops[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%zu: %s - %s\n", i, stops[i].stop_name, stops[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(stops[i].stop_name);
        free(stops[i].note);
    }
    free(stops);
    free(line);
    return 0;
}
