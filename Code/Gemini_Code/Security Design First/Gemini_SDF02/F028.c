/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

int main(void) {
    Stop *route = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Stop *temp = realloc(route, new_cap * sizeof(Stop));
                if (!temp) break;
                route = temp;
                capacity = new_cap;
            }
            route[count].stop = strdup(stop);
            route[count].note = strdup(note);
            if (route[count].stop && route[count].note) count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;

            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Stop *temp = realloc(route, new_cap * sizeof(Stop));
                    if (!temp) break;
                    route = temp;
                    capacity = new_cap;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    route[i] = route[i - 1];
                }
                route[idx].stop = strdup(stop);
                route[idx].note = strdup(note);
                if (route[idx].stop && route[idx].note) count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int to_idx = atoi(p);
            int from_idx = atoi(from_str);

            if (from_idx >= 0 && (size_t)from_idx < count && to_idx >= 0 && (size_t)to_idx < count && from_idx != to_idx) {
                Stop target = route[from_idx];
                if (from_idx < to_idx) {
                    for (size_t i = (size_t)from_idx; i < (size_t)to_idx; i++) {
                        route[i] = route[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)from_idx; i > (size_t)to_idx; i--) {
                        route[i] = route[i - 1];
                    }
                }
                route[to_idx] = target;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = atoi(p);
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
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(route[i].stop);
        free(route[i].note);
    }
    free(route);
    return 0;
}
