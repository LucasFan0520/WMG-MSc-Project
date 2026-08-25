// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int line_count = 0, line_cap = 0;

void insert_line(int idx, const char *text) {
    if (idx < 0 || idx > line_count) return;
    if (line_count == line_cap) {
        line_cap = line_cap ? line_cap * 2 : 4;
        lines = realloc(lines, sizeof(char *) * line_cap);
    }
    for (int i = line_count; i > idx; i--) lines[i] = lines[i - 1];
    lines[idx] = strdup(text);
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int idx) {
    if (idx < 0 || idx >= line_count) return;
    free(lines[idx]);
    for (int i = idx; i < line_count - 1; i++) lines[i] = lines[i + 1];
    line_count--;
}

void patch_line(int idx, const char *text) {
    if (idx < 0 || idx >= line_count) return;
    free(lines[idx]);
    lines[idx] = strdup(text);
}

void print_lines(void) {
    for (int i = 0; i < line_count; i++) {
        for (char *p = lines[i]; *p; p++) putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], rest[4096];
        int idx;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %4095[^\n]", &idx, rest) == 2)
                insert_line(idx, rest);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(line, "%*s %d %4095[^\n]", &idx, rest) == 2)
                patch_line(idx, rest);
        } else if (strcmp(cmd, "PRINT") == 0) print_lines();
    }
    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);
    return 0;
}