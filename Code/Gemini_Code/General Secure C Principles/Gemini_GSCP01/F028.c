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
    RouteStop *stops = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            RouteStop *temp = realloc(stops, (count + 1) * sizeof(RouteStop));
            if (temp) {
                stops = temp;
                stops[count].stop = strdup(stop);
                stops[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx <= count) {
                RouteStop *temp = realloc(stops, (count + 1) * sizeof(RouteStop));
                if (temp) {
                    stops = temp;
                    for (size_t i = count; i > (size_t)idx; i--) {
                        stops[i] = stops[i - 1];
                    }
                    stops[idx].stop = strdup(stop);
                    stops[idx].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *from_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *to_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int from_idx = atoi(from_s);
            int to_idx = atoi(to_s);
            if (from_idx >= 0 && (size_t)from_idx < count && to_idx >= 0 && (size_t)to_idx < count && from_idx != to_idx) {
                RouteStop target = stops[from_idx];
                if ((size_t)from_idx < (size_t)to_idx) {
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
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(stops[idx].stop);
                free(stops[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    stops[i] = stops[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(stops[i].stop, stop) == 0) {
                    printf("%s\n", stops[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%zu] %s: %s\n", i, stops[i].stop, stops[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
    free(line);
    return 0;
}
