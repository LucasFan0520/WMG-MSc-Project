// F028.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

int main(void) {
    Stop *route = NULL;
    int count = 0;
    int capacity = 0;
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
            while (*p == ' ') p++;
            char *stop = p;
            char *space = strchr(stop, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
                while (*note == ' ') note++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Stop *tmp = realloc(route, new_cap * sizeof(Stop));
                if (!tmp) return 1;
                route = tmp;
                capacity = new_cap;
            }
            route[count].stop = strdup(stop);
            route[count].note = strdup(note);
            count++;
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (p == endptr || idx < 0 || idx > count) continue;
            while (*endptr == ' ') endptr++;
            char *stop = endptr;
            char *space = strchr(stop, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
                while (*note == ' ') note++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Stop *tmp = realloc(route, new_cap * sizeof(Stop));
                if (!tmp) return 1;
                route = tmp;
                capacity = new_cap;
            }
            for (int i = count; i > idx; i--) {
                route[i] = route[i - 1];
            }
            route[idx].stop = strdup(stop);
            route[idx].note = strdup(note);
            count++;
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *endptr1;
            long from = strtol(p, &endptr1, 10);
            if (p == endptr1 || from < 0 || from >= count) continue;
            while (*endptr1 == ' ') endptr1++;
            char *endptr2;
            long to = strtol(endptr1, &endptr2, 10);
            if (endptr1 == endptr2 || to < 0 || to >= count) continue;
            if (from == to) continue;
            Stop target = route[from];
            if (from < to) {
                for (int i = from; i < to; i++) {
                    route[i] = route[i + 1];
                }
            } else {
                for (int i = from; i > to; i--) {
                    route[i] = route[i - 1];
                }
            }
            route[to] = target;
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (p == endptr || idx < 0 || idx >= count) continue;
            free(route[idx].stop);
            free(route[idx].note);
            for (int i = idx; i < count - 1; i++) {
                route[i] = route[i + 1];
            }
            count--;
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            while (*stop == ' ') stop++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", route[found].note);
            else printf("NOT_FOUND\n");
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", route[i].stop, route[i].note);
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
