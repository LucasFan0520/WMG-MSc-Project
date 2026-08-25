// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *stop;
    char *note;
} Stop;

int main(void) {
    Stop *route = NULL;
    int count = 0;
    int capacity = 0;
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
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Stop *new_r = realloc(route, capacity * sizeof(Stop));
                    if (new_r) route = new_r;
                }
                if (count < capacity) {
                    route[count].stop = strdup(stop);
                    route[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx <= count) {
                p = endptr;
                while (*p == ' ') p++;
                char *stop = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Stop *new_r = realloc(route, capacity * sizeof(Stop));
                        if (new_r) route = new_r;
                    }
                    if (count < capacity) {
                        for (int j = count; j > idx; j--) {
                            route[j] = route[j - 1];
                        }
                        route[idx].stop = strdup(stop);
                        route[idx].note = strdup(note);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *endptr;
            long from = strtol(p, &endptr, 10);
            if (endptr != p && from >= 0 && from < count) {
                p = endptr;
                while (*p == ' ') p++;
                long to = strtol(p, &endptr, 10);
                if (endptr != p && to >= 0 && to < count && from != to) {
                    Stop temp = route[from];
                    if (from < to) {
                        for (int j = from; j < to; j++) {
                            route[j] = route[j + 1];
                        }
                    } else {
                        for (int j = from; j > to; j--) {
                            route[j] = route[j - 1];
                        }
                    }
                    route[to] = temp;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx < count) {
                free(route[idx].stop);
                free(route[idx].note);
                for (int j = idx; j < count - 1; j++) {
                    route[j] = route[j + 1];
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
                printf("%d: %s (%s)\n", i, route[i].stop, route[i].note);
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
