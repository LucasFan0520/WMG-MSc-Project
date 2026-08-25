// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *route = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *stop = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                RouteStop *nb = realloc(route, cap * sizeof(RouteStop));
                if (!nb) {
                    free(line);
                    break;
                }
                route = nb;
            }
            route[count].stop = safe_dup(stop);
            route[count].note = safe_dup(note);
            count++;
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *stop = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            long long idx = strtoll(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    RouteStop *nb = realloc(route, cap * sizeof(RouteStop));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    route = nb;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    route[i] = route[i - 1];
                }
                route[idx].stop = safe_dup(stop);
                route[idx].note = safe_dup(note);
                count++;
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            char *from_str = p;
            while (*p && *p != ' ') p++;
            char *to_str = "";
            if (*p == ' ') {
                *p = '\0';
                to_str = p + 1;
            }
            long long from_idx = strtoll(from_str, NULL, 10);
            long long to_idx = strtoll(to_str, NULL, 10);
            if (from_idx >= 0 && (size_t)from_idx < count && to_idx >= 0 && (size_t)to_idx < count) {
                RouteStop tmp = route[from_idx];
                if (from_idx < to_idx) {
                    for (size_t i = (size_t)from_idx; i < (size_t)to_idx; i++) {
                        route[i] = route[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)from_idx; i > (size_t)to_idx; i--) {
                        route[i] = route[i - 1];
                    }
                }
                route[to_idx] = tmp;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *idx_str = line + 7;
            long long idx = strtoll(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s - %s\n", route[i].stop, route[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
