// F028.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

RouteStop *route = NULL;
int count = 0;
int capacity = 0;

void add_stop(const char *stop, const char *note) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        route = realloc(route, capacity * sizeof(RouteStop));
    }
    route[count].stop = strdup(stop);
    route[count].note = strdup(note);
    count++;
}

void insert_stop(int idx, const char *stop, const char *note) {
    if (idx < 0 || idx > count) return;
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        route = realloc(route, capacity * sizeof(RouteStop));
    }
    for (int i = count; i > idx; i--) {
        route[i] = route[i - 1];
    }
    route[idx].stop = strdup(stop);
    route[idx].note = strdup(note);
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count) return;
    if (from == to) return;
    RouteStop temp = route[from];
    if (from < to) {
        for (int i = from; i < to; i++) {
            route[i] = route[i + 1];
        }
    } else {
        for (int i = from; i > to; i--) {
            route[i] = route[i - 1];
        }
    }
    route[to] = temp;
}

void remove_stop(int idx) {
    if (idx < 0 || idx >= count) return;
    free(route[idx].stop);
    free(route[idx].note);
    for (int i = idx; i < count - 1; i++) {
        route[i] = route[i + 1];
    }
    count--;
}

void find_stop(const char *stop) {
    for (int i = 0; i < count; i++) {
        if (strcmp(route[i].stop, stop) == 0) {
            printf("%s\n", route[i].note);
            return;
        }
    }
}

void print_route(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", route[i].stop, route[i].note);
    }
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (stop && note) add_stop(stop, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = strtok(NULL, " ");
            char *stop = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (idx_s && stop && note) {
                insert_stop(atoi(idx_s), stop, note);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_s = strtok(NULL, " ");
            char *to_s = strtok(NULL, "");
            if (from_s && to_s) {
                move_stop(atoi(from_s), atoi(to_s));
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_s = strtok(NULL, "");
            if (idx_s) remove_stop(atoi(idx_s));
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = strtok(NULL, "");
            if (stop) find_stop(stop);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_route();
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
