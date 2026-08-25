// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0, cap = 0;

void free_lines() {
    for (int i = 0; i < count; i++) free(lines[i]);
    free(lines);
}

void print_spaces(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], text[4096] = "";
        int idx;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %4095[^\n]", &idx, text) < 1) continue;
            if (idx < 0 || idx > count) continue;
            if (count == cap) {
                cap = cap ? cap * 2 : 4;
                lines = realloc(lines, cap * sizeof(char*));
            }
            for (int i = count; i > idx; i--) lines[i] = lines[i-1];
            lines[idx] = strdup(text);
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", text) != 1) continue;
            if (count == cap) {
                cap = cap ? cap * 2 : 4;
                lines = realloc(lines, cap * sizeof(char*));
            }
            lines[count++] = strdup(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &idx) != 1) continue;
            if (idx < 0 || idx >= count) continue;
            free(lines[idx]);
            for (int i = idx; i < count - 1; i++) lines[i] = lines[i+1];
            count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(line, "%*s %d %4095[^\n]", &idx, text) < 1) continue;
            if (idx < 0 || idx >= count) continue;
            free(lines[idx]);
            lines[idx] = strdup(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) print_spaces(lines[i]);
        }
    }
    free_lines();
    return 0;
}