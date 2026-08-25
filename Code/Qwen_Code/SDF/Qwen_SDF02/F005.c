// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char **lines = NULL;
size_t count = 0, capacity = 0;

void insert_line(size_t idx, const char *text) {
    if (idx > count) return;
    if (count == capacity) {
        size_t new_cap = capacity ? capacity * 2 : 4;
        char **new_lines = realloc(lines, new_cap * sizeof(char*));
        if (!new_lines) return;
        lines = new_lines;
        capacity = new_cap;
    }
    for (size_t i = count; i > idx; i--) lines[i] = lines[i-1];
    lines[idx] = strdup(text);
    count++;
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
        
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            int n = 0;
            if (sscanf(rest, "%d%n", &idx, &n) != 1 || idx < 0) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            insert_line((size_t)idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            insert_line(count, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            if (sscanf(rest, "%d", &idx) != 1 || idx < 0 || (size_t)idx >= count) continue;
            free(lines[idx]);
            for (size_t i = (size_t)idx; i < count - 1; i++) lines[i] = lines[i+1];
            count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            int n = 0;
            if (sscanf(rest, "%d%n", &idx, &n) != 1 || idx < 0 || (size_t)idx >= count) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            free(lines[idx]);
            lines[idx] = strdup(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (const char *p = lines[i]; *p; p++) putchar(*p == ' ' ? '_' : *p);
                putchar('\n');
            }
        }
    }
    for (size_t i = 0; i < count; i++) free(lines[i]);
    free(lines);
    return 0;
}