// F028.c
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
    if (idx < 0 || idx > count) {
        return;
    }
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                add_stop(p1, sp + 1);
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p1 = line + 7;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    insert_stop(atoi(p1), p2, sp2 + 1);
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p1 = line + 5;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                int from_idx = atoi(p1);
                int to_idx = atoi(sp + 1);
                if (from_idx >= 0 && from_idx < count && to_idx >= 0 && to_idx < count) {
                    RouteStop temp = route[from_idx];
                    if (from_idx < to_idx) {
                        for (int i = from_idx; i < to_idx; i++) {
                            route[i] = route[i + 1];
                        }
                    } else {
                        for (int i = from_idx; i > to_idx; i--) {
                            route[i] = route[i - 1];
                        }
                    }
                    route[to_idx] = temp;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", route[i].stop, route[i].note);
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
