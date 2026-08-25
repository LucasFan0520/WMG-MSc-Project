// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *route = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = p;
            while (*stop == ' ') stop++;
            char *stop_end = stop;
            while (*stop_end && *stop_end != ' ') stop_end++;
            char *note = stop_end;
            if (*stop_end != '\0') {
                *stop_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                RouteStop *new_r = realloc(route, capacity * sizeof(RouteStop));
                if (new_r) route = new_r;
            }
            route[count].stop = mystrdup(stop);
            route[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = p;
            while (*idx_s == ' ') idx_s++;
            char *idx_end = idx_s;
            while (*idx_end && *idx_end != ' ') idx_end++;
            char *stop = idx_end;
            if (*idx_end != '\0') {
                *idx_end = '\0';
                stop++;
            }
            while (*stop == ' ') stop++;
            char *stop_end = stop;
            while (*stop_end && *stop_end != ' ') stop_end++;
            char *note = stop_end;
            if (*stop_end != '\0') {
                *stop_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *new_r = realloc(route, capacity * sizeof(RouteStop));
                    if (new_r) route = new_r;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    route[i] = route[i - 1];
                }
                route[idx].stop = mystrdup(stop);
                route[idx].note = mystrdup(note);
                count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *f_s = p;
            while (*f_s == ' ') f_s++;
            char *f_end = f_s;
            while (*f_end && *f_end != ' ') f_end++;
            char *t_s = f_end;
            if (*f_end != '\0') {
                *f_end = '\0';
                t_s++;
            }
            while (*t_s == ' ') t_s++;
            char *t_end = t_s;
            while (*t_end && *t_end != ' ') t_end++;
            *t_end = '\0';
            int f_idx = atoi(f_s);
            int t_idx = atoi(t_s);
            if (f_idx >= 0 && (size_t)f_idx < count && t_idx >= 0 && (size_t)t_idx < count && f_idx != t_idx) {
                RouteStop temp = route[f_idx];
                if (f_idx < t_idx) {
                    for (size_t i = (size_t)f_idx; i < (size_t)t_idx; i++) {
                        route[i] = route[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)f_idx; i > (size_t)t_idx; i--) {
                        route[i] = route[i - 1];
                    }
                }
                route[t_idx] = temp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_s = p;
            while (*idx_s == ' ') idx_s++;
            char *idx_end = idx_s;
            while (*idx_end && *idx_end != ' ') idx_end++;
            *idx_end = '\0';
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = p;
            while (*stop == ' ') stop++;
            char *stop_end = stop;
            while (*stop_end && *stop_end != ' ') stop_end++;
            *stop_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", route[i].stop, route[i].note);
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
