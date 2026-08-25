// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *stop;
    char *note;
} Stop;

int main() {
    Stop *stops = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s: %s\n", stops[i].stop, stops[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                stops = realloc(stops, sizeof(Stop) * (count + 1));
                stops[count].stop = strdup(args);
                stops[count].note = strdup(p2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int idx = atoi(args);
                char *stop = p2 + 1;
                char *p3 = strchr(stop, ' ');
                if (p3 && idx >= 0 && idx <= count) {
                    *p3 = '\0';
                    stops = realloc(stops, sizeof(Stop) * (count + 1));
                    for (int j = count; j > idx; j--) {
                        stops[j] = stops[j - 1];
                    }
                    stops[idx].stop = strdup(stop);
                    stops[idx].note = strdup(p3 + 1);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int from = atoi(args);
                int to = atoi(p2 + 1);
                if (from >= 0 && from < count && to >= 0 && to <= count) {
                    Stop tmp = stops[from];
                    if (from < to) {
                        for (int j = from; j < to - 1; j++) {
                            stops[j] = stops[j + 1];
                        }
                        stops[to - 1] = tmp;
                    } else if (from > to) {
                        for (int j = from; j > to; j--) {
                            stops[j] = stops[j - 1];
                        }
                        stops[to] = tmp;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = atoi(args);
            if (idx >= 0 && idx < count) {
                free(stops[idx].stop);
                free(stops[idx].note);
                for (int j = idx; j < count - 1; j++) {
                    stops[j] = stops[j + 1];
                }
                count--;
                if (count == 0) { free(stops); stops = NULL; }
                else { stops = realloc(stops, sizeof(Stop) * count); }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(stops[i].stop, args) == 0) {
                    printf("%s\n", stops[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}
