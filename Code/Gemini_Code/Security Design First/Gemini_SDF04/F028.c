// F028.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

RouteStop *stops = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *note = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        RouteStop *tmp = realloc(stops, capacity * sizeof(RouteStop));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        stops = tmp;
                    }
                    stops[count].stop = strdup(arg1);
                    stops[count].note = strdup(note);
                    if (stops[count].stop && stops[count].note) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "INSERT") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int idx = atoi(arg1);
                    char *stop_name = space2 + 1;
                    char *space3 = strchr(stop_name, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *note = space3 + 1;
                        if (idx >= 0 && idx <= count) {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                RouteStop *tmp = realloc(stops, capacity * sizeof(RouteStop));
                                if (!tmp) {
                                    free(line);
                                    return 1;
                                }
                                stops = tmp;
                            }
                            for (int i = count; i > idx; i--) {
                                stops[i] = stops[i - 1];
                            }
                            stops[idx].stop = strdup(stop_name);
                            stops[idx].note = strdup(note);
                            if (stops[idx].stop && stops[idx].note) {
                                count++;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "MOVE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int from = atoi(arg1);
                    int to = atoi(space2 + 1);
                    if (from >= 0 && from < count && to >= 0 && to < count && from != to) {
                        RouteStop target = stops[from];
                        if (from < to) {
                            for (int i = from; i < to; i++) {
                                stops[i] = stops[i + 1];
                            }
                        } else {
                            for (int i = from; i > to; i--) {
                                stops[i] = stops[i - 1];
                            }
                        }
                        stops[to] = target;
                    }
                }
            } else if (strcmp(cmd, "REMOVE") == 0) {
                int idx = atoi(arg1);
                if (idx >= 0 && idx < count) {
                    free(stops[idx].stop);
                    free(stops[idx].note);
                    for (int i = idx; i < count - 1; i++) {
                        stops[i] = stops[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(stops[i].stop, arg1) == 0) {
                        printf("%s\n", stops[i].note);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", stops[i].stop, stops[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}