// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].stop, arr[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = args;
            char *p2 = strchr(stop, ' ');
            if (p2) {
                *p2 = '\0';
                char *note = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    RouteStop *narr = realloc(arr, ncap * sizeof(RouteStop));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].stop = strdup(stop);
                    arr[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = args;
            char *p2 = strchr(idx_s, ' ');
            if (p2) {
                *p2 = '\0';
                char *stop = p2 + 1;
                char *p3 = strchr(stop, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    long idx = strtol(idx_s, NULL, 10);
                    if (idx >= 0 && idx <= (long)count) {
                        if (count >= cap) {
                            size_t ncap = cap == 0 ? 4 : cap * 2;
                            RouteStop *narr = realloc(arr, ncap * sizeof(RouteStop));
                            if (narr) {
                                arr = narr;
                                cap = ncap;
                            }
                        }
                        if (count < cap) {
                            for (size_t i = count; i > (size_t)idx; i--) {
                                arr[i] = arr[i - 1];
                            }
                            arr[idx].stop = strdup(stop);
                            arr[idx].note = strdup(note);
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_s = args;
            char *p2 = strchr(from_s, ' ');
            if (p2) {
                *p2 = '\0';
                char *to_s = p2 + 1;
                long from = strtol(from_s, NULL, 10);
                long to = strtol(to_s, NULL, 10);
                if (from >= 0 && from < (long)count && to >= 0 && to < (long)count) {
                    RouteStop tmp = arr[from];
                    if (from < to) {
                        for (size_t i = (size_t)from; i < (size_t)to; i++) {
                            arr[i] = arr[i + 1];
                        }
                    } else if (from > to) {
                        for (size_t i = (size_t)from; i > (size_t)to; i--) {
                            arr[i] = arr[i - 1];
                        }
                    }
                    arr[to] = tmp;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            long idx = strtol(args, NULL, 10);
            if (idx >= 0 && idx < (long)count) {
                free(arr[idx].stop);
                free(arr[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].stop, stop) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].stop);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
