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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *stop;
    char *note;
} Stop;

int main(void) {
    Stop *route = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    route = realloc(route, cap * sizeof(Stop));
                }
                route[count].stop = strdup(stop);
                route[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            p = endptr;
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ' && idx >= 0 && idx <= count) {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    route = realloc(route, cap * sizeof(Stop));
                }
                for (int i = count; i > idx; i--) {
                    route[i] = route[i - 1];
                }
                route[idx].stop = strdup(stop);
                route[idx].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *endptr;
            long from = strtol(p, &endptr, 10);
            p = endptr;
            while (*p == ' ') p++;
            long to = strtol(p, NULL, 10);
            if (from >= 0 && from < count && to >= 0 && to < count) {
                Stop tmp = route[from];
                if (from < to) {
                    for (int i = from; i < to; i++) route[i] = route[i + 1];
                } else {
                    for (int i = from; i > to; i--) route[i] = route[i - 1];
                }
                route[to] = tmp;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            long idx = strtol(line + 7, NULL, 10);
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
            while (*stop == ' ') stop++;
            for (int i = 0; i < count; i++) {
                if (strcmp(route[i].stop, stop) == 0) {
                    printf("%s\n", route[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s - %s\n", route[i].stop, route[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
