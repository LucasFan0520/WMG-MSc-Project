// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

int main() {
    Stop *route = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (stop && note) {
                route = realloc(route, (count + 1) * sizeof(Stop));
                route[count].stop = strdup(stop);
                route[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = strtok(NULL, " ");
            char *stop = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (idx_str && stop && note) {
                int idx = atoi(idx_str);
                if (idx >= 0 && idx <= count) {
                    route = realloc(route, (count + 1) * sizeof(Stop));
                    for (int i = count; i > idx; i--) {
                        route[i] = route[i - 1];
                    }
                    route[idx].stop = strdup(stop);
                    route[idx].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_str = strtok(NULL, " ");
            char *to_str = strtok(NULL, " ");
            if (from_str && to_str) {
                int fromIdx = atoi(from_str);
                int toIdx = atoi(to_str);
                if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count && fromIdx != toIdx) {
                    Stop temp = route[fromIdx];
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
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_str = strtok(NULL, " ");
            if (idx_str) {
                int idx = atoi(idx_str);
                if (idx >= 0 && idx < count) {
                    free(route[idx].stop);
                    free(route[idx].note);
                    for (int i = idx; i < count - 1; i++) {
                        route[i] = route[i + 1];
                    }
                    count--;
                    if (count == 0) {
                        free(route);
                        route = NULL;
                    } else {
                        route = realloc(route, count * sizeof(Stop));
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = strtok(NULL, " ");
            if (stop) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(route[i].stop, stop) == 0) {
                        printf("%s\n", route[i].note);
                        break;
                    }
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
