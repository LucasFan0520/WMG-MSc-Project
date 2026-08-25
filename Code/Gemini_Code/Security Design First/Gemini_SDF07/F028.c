/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *route = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *stp = NULL;
            char *nt = NULL;
            if (space) {
                *space = '\0';
                stp = args;
                nt = space + 1;
            } else {
                stp = args;
                nt = "";
            }
            char *n_stp = strdup(stp);
            char *n_nt = strdup(nt);
            if (n_stp && n_nt) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *tmp = realloc(route, new_cap * sizeof(RouteStop));
                    if (tmp) {
                        route = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_stp); free(n_nt);
                        if (space) *space = ' ';
                        continue;
                    }
                }
                route[count].stop = n_stp;
                route[count].note = n_nt;
                count++;
            } else {
                free(n_stp); free(n_nt);
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *args = line + 7;
            char *space1 = strchr(args, ' ');
            if (space1) {
                *space1 = '\0';
                int idx = atoi(args);
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                char *stp = NULL;
                char *nt = NULL;
                if (space2) {
                    *space2 = '\0';
                    stp = rest;
                    nt = space2 + 1;
                } else {
                    stp = rest;
                    nt = "";
                }
                if (idx >= 0 && idx <= count) {
                    char *n_stp = strdup(stp);
                    char *n_nt = strdup(nt);
                    if (n_stp && n_nt) {
                        if (count >= capacity) {
                            int new_cap = capacity == 0 ? 4 : capacity * 2;
                            RouteStop *tmp = realloc(route, new_cap * sizeof(RouteStop));
                            if (tmp) {
                                route = tmp;
                                capacity = new_cap;
                            } else {
                                free(n_stp); free(n_nt);
                                *space1 = ' '; if (space2) *space2 = ' ';
                                continue;
                            }
                        }
                        for (int i = count; i > idx; i--) {
                            route[i] = route[i - 1];
                        }
                        route[idx].stop = n_stp;
                        route[idx].note = n_nt;
                        count++;
                    } else {
                        free(n_stp); free(n_nt);
                    }
                }
                *space1 = ' ';
                if (space2) *space2 = ' ';
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                int fromI = atoi(args);
                int toI = atoi(space + 1);
                if (fromI >= 0 && fromI < count && toI >= 0 && toI < count && fromI != toI) {
                    RouteStop target = route[fromI];
                    if (fromI < toI) {
                        for (int i = fromI; i < toI; i++) {
                            route[i] = route[i + 1];
                        }
                    } else {
                        for (int i = fromI; i > toI; i--) {
                            route[i] = route[i - 1];
                        }
                    }
                    route[toI] = target;
                }
                *space = ' ';
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
            char *stp = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stp) == 0) {
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
    free(line);
    for (int i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
