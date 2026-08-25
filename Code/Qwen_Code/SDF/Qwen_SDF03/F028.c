// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct {
    char *stop;
    char *note;
} Stop;

Stop *stops = NULL;
size_t count = 0, cap = 0;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

void add_stop(size_t idx, const char *stop, const char *note) {
    if (idx > count) idx = count;
    if (count == cap) {
        size_t new_cap = cap ? cap * 2 : 4;
        Stop *tmp = realloc(stops, new_cap * sizeof(Stop));
        if (!tmp) return;
        stops = tmp;
        cap = new_cap;
    }
    for (size_t i = count; i > idx; i--) stops[i] = stops[i - 1];
    stops[idx].stop = strdup(stop);
    stops[idx].note = strdup(note);
    count++;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = get_token(&ptr);
            if (stop) add_stop(count, stop, ptr);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *istr = get_token(&ptr);
            char *stop = get_token(&ptr);
            if (istr && stop) {
                size_t idx = strtoul(istr, NULL, 10);
                if (idx <= count) add_stop(idx, stop, ptr);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *fstr = get_token(&ptr);
            char *tstr = get_token(&ptr);
            if (fstr && tstr) {
                size_t from = strtoul(fstr, NULL, 10);
                size_t to = strtoul(tstr, NULL, 10);
                if (from < count && to < count && from != to) {
                    Stop s = stops[from];
                    if (from < to) {
                        for (size_t i = from; i < to; i++) stops[i] = stops[i + 1];
                    } else {
                        for (size_t i = from; i > to; i--) stops[i] = stops[i - 1];
                    }
                    stops[to] = s;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *istr = get_token(&ptr);
            if (istr) {
                size_t idx = strtoul(istr, NULL, 10);
                if (idx < count) {
                    free(stops[idx].stop);
                    free(stops[idx].note);
                    for (size_t i = idx; i < count - 1; i++) stops[i] = stops[i + 1];
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = get_token(&ptr);
            if (stop) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(stops[i].stop, stop) == 0) {
                        printf("%s\n", stops[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}