// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main() {
    RouteStop *route = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char stop[256];
            int bytes = 0;
            if (sscanf(args, "%255s%n", stop, &bytes) == 1) {
                char *note = args + bytes;
                while (*note == ' ') note++;
                route = realloc(route, (count + 1) * sizeof(RouteStop));
                route[count].stop = strdup(stop);
                route[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx = 0;
            char stop[256];
            int bytes = 0;
            if (sscanf(args, "%d %255s%n", &idx, stop, &bytes) == 2) {
                if (idx >= 0 && idx <= count) {
                    char *note = args + bytes;
                    while (*note == ' ') note++;
                    route = realloc(route, (count + 1) * sizeof(RouteStop));
                    for (int i = count; i > idx; i--) {
                        route[i] = route[i - 1];
                    }
                    route[idx].stop = strdup(stop);
                    route[idx].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from = 0, to = 0;
            if (sscanf(args, "%d %d", &from, &to) == 2) {
                if (from >= 0 && from < count && to >= 0 && to < count) {
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
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = 0;
            if (sscanf(args, "%d", &idx) == 1) {
                if (idx >= 0 && idx < count) {
                    free(route[idx].stop);
                    free(route[idx].note);
                    for (int i = idx; i < count - 1; i++) {
                        route[i] = route[i + 1];
                    }
                    count--;
                    route = realloc(route, count * sizeof(RouteStop));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[256];
            if (sscanf(args, "%255s", stop) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(route[i].stop, stop) == 0) {
                        printf("%s\n", route[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s - %s\n", route[i].stop, route[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
