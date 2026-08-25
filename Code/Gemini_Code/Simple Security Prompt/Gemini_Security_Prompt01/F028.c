// F028.c
#define _GNU_SOURCE
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *stop = args;
            char *space2 = strchr(args, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            route = realloc(route, (count + 1) * sizeof(RouteStop));
            if (!route) return 1;
            route[count].stop = strdup(stop);
            route[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0 && args) {
            char *idx_str = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int index = atoi(idx_str);
                char *stop = space2 + 1;
                while (*stop == ' ') stop++;
                char *space3 = strchr(stop, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                if (index >= 0 && index <= count) {
                    route = realloc(route, (count + 1) * sizeof(RouteStop));
                    if (!route) return 1;
                    for (int j = count; j > index; j--) {
                        route[j] = route[j - 1];
                    }
                    route[index].stop = strdup(stop);
                    route[index].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0 && args) {
            char *from_str = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int fromIdx = atoi(from_str);
                int toIdx = atoi(space2 + 1);
                if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count) {
                    RouteStop temp = route[fromIdx];
                    if (fromIdx < toIdx) {
                        for (int j = fromIdx; j < toIdx; j++) {
                            route[j] = route[j + 1];
                        }
                    } else {
                        for (int j = fromIdx; j > toIdx; j--) {
                            route[j] = route[j - 1];
                        }
                    }
                    route[toIdx] = temp;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && args) {
            int index = atoi(args);
            if (index >= 0 && index < count) {
                free(route[index].stop);
                free(route[index].note);
                for (int j = index; j < count - 1; j++) {
                    route[j] = route[j + 1];
                }
                count--;
                route = realloc(route, count * sizeof(RouteStop));
                if (count > 0 && !route) return 1;
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, args) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
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
