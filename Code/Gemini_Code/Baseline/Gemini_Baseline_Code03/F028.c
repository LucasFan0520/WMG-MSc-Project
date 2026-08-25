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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char stop[256];
            int s_len = 0;
            if (sscanf(ptr, "%255s%n", stop, &s_len) > 0) {
                char *note = ptr + s_len;
                while (*note == ' ') note++;
                route = realloc(route, (count + 1) * sizeof(RouteStop));
                route[count].stop = strdup(stop);
                route[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx = -1;
            char stop[256];
            int idx_len = 0, s_len = 0;
            if (sscanf(ptr, "%d%n", &idx, &idx_len) > 0) {
                char *ptr2 = ptr + idx_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%255s%n", stop, &s_len) > 0) {
                    if (idx >= 0 && idx <= count) {
                        char *note = ptr2 + s_len;
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
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from = -1, to = -1;
            if (sscanf(ptr, "%d %d", &from, &to) == 2) {
                if (from >= 0 && from < count && to >= 0 && to <= count) {
                    RouteStop target = route[from];
                    if (from < to) {
                        for (int i = from; i < to - 1; i++) {
                            route[i] = route[i + 1];
                        }
                        route[to - 1] = target;
                    } else if (from > to) {
                        for (int i = from; i > to; i--) {
                            route[i] = route[i - 1];
                        }
                        route[to] = target;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = -1;
            if (sscanf(ptr, "%d", &idx) > 0) {
                if (idx >= 0 && idx < count) {
                    free(route[idx].stop);
                    free(route[idx].note);
                    for (int i = idx; i < count - 1; i++) {
                        route[i] = route[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        route = realloc(route, count * sizeof(RouteStop));
                    } else {
                        free(route);
                        route = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[256];
            if (sscanf(ptr, "%255s", stop) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(route[i].stop, stop) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", route[found].note);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d: %s - %s\n", i, route[i].stop, route[i].note);
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
