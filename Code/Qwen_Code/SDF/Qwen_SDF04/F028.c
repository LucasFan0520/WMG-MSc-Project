// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* note; } Stop;

Stop* stops = NULL;
size_t s_count = 0, s_cap = 0;

void cleanup() {
    for (size_t i = 0; i < s_count; i++) {
        free(stops[i].name);
        free(stops[i].note);
    }
    free(stops);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;

            if (s_count == s_cap) {
                size_t new_cap = s_cap == 0 ? 4 : s_cap * 2;
                Stop* temp = realloc(stops, new_cap * sizeof(Stop));
                if (!temp) { cleanup(); free(line); return 1; }
                stops = temp; s_cap = new_cap;
            }
            stops[s_count].name = strdup(name);
            stops[s_count].note = strdup(note);
            if (!stops[s_count].name || !stops[s_count].note) { cleanup(); free(line); return 1; }
            s_count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* idx_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;

            long idx = strtol(idx_str, NULL, 10);
            if (idx < 0 || idx > (long)s_count) continue;

            if (s_count == s_cap) {
                size_t new_cap = s_cap == 0 ? 4 : s_cap * 2;
                Stop* temp = realloc(stops, new_cap * sizeof(Stop));
                if (!temp) { cleanup(); free(line); return 1; }
                stops = temp; s_cap = new_cap;
            }
            for (size_t i = s_count; i > (size_t)idx; i--) stops[i] = stops[i - 1];
            stops[idx].name = strdup(name);
            stops[idx].note = strdup(note);
            if (!stops[idx].name || !stops[idx].note) { cleanup(); free(line); return 1; }
            s_count++;
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* from_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* to_str = ptr;

            long from = strtol(from_str, NULL, 10);
            long to = strtol(to_str, NULL, 10);
            if (from < 0 || from >= (long)s_count || to < 0 || to >= (long)s_count) continue;

            Stop temp = stops[from];
            if (from < to) {
                for (size_t i = from; i < (size_t)to; i++) stops[i] = stops[i + 1];
            } else {
                for (size_t i = from; i > (size_t)to; i--) stops[i] = stops[i - 1];
            }
            stops[to] = temp;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            long idx = strtol(ptr, NULL, 10);
            if (idx < 0 || idx >= (long)s_count) continue;

            free(stops[idx].name);
            free(stops[idx].note);
            for (size_t i = idx; i < s_count - 1; i++) stops[i] = stops[i + 1];
            s_count--;
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < s_count; i++) {
                if (strcmp(stops[i].name, ptr) == 0) {
                    printf("%s\n", stops[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < s_count; i++) {
                printf("%s %s\n", stops[i].name, stops[i].note);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}