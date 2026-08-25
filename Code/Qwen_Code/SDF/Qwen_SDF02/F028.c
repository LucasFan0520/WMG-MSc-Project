// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Stop {
    char *name;
    char *note;
} Stop;

Stop *stops = NULL;
size_t count = 0, capacity = 0;

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            if (count == capacity) {
                size_t new_cap = capacity ? capacity * 2 : 4;
                Stop *tmp = realloc(stops, new_cap * sizeof(Stop));
                if (!tmp) continue;
                stops = tmp;
                capacity = new_cap;
            }
            stops[count].name = strdup(name);
            stops[count].note = strdup(note);
            if (!stops[count].name || !stops[count].note) {
                free(stops[count].name); free(stops[count].note); continue;
            }
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char name[256];
            int n = 0;
            if (sscanf(rest, "%d %255s%n", &idx, name, &n) != 2) continue;
            if (idx < 0 || (size_t)idx > count) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            if (count == capacity) {
                size_t new_cap = capacity ? capacity * 2 : 4;
                Stop *tmp = realloc(stops, new_cap * sizeof(Stop));
                if (!tmp) continue;
                stops = tmp;
                capacity = new_cap;
            }
            for (size_t i = count; i > (size_t)idx; i--) stops[i] = stops[i-1];
            stops[idx].name = strdup(name);
            stops[idx].note = strdup(note);
            if (!stops[idx].name || !stops[idx].note) {
                free(stops[idx].name); free(stops[idx].note); continue;
            }
            count++;
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            if (sscanf(rest, "%d %d", &from, &to) != 2) continue;
            if (from < 0 || (size_t)from >= count || to < 0 || (size_t)to >= count) continue;
            Stop tmp = stops[from];
            if (from < to) {
                for (int i = from; i < to; i++) stops[i] = stops[i+1];
            } else {
                for (int i = from; i > to; i--) stops[i] = stops[i-1];
            }
            stops[to] = tmp;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            if (sscanf(rest, "%d", &idx) != 1) continue;
            if (idx < 0 || (size_t)idx >= count) continue;
            free(stops[idx].name);
            free(stops[idx].note);
            for (size_t i = (size_t)idx; i < count - 1; i++) stops[i] = stops[i+1];
            count--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(stops[i].name, name) == 0) {
                    print_text(stops[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s ", stops[i].name);
                print_text(stops[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(stops[i].name);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}