/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int line_count = 0;
int capacity = 0;

void ensure_capacity(int n) {
    if (n >= capacity) {
        capacity = capacity ? capacity * 2 : 4;
        while (capacity <= n) capacity *= 2;
        lines = realloc(lines, capacity * sizeof(char *));
    }
}

void insert_line(int idx, const char *text) {
    if (idx < 0) return;
    if (idx > line_count) idx = line_count;
    ensure_capacity(line_count + 1);
    for (int i = line_count; i > idx; i--) {
        lines[i] = lines[i-1];
    }
    lines[idx] = strdup(text);
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int idx) {
    if (idx < 0 || idx >= line_count) return;
    free(lines[idx]);
    for (int i = idx; i < line_count - 1; i++) {
        lines[i] = lines[i+1];
    }
    line_count--;
}

void patch_line(int idx, const char *text) {
    if (idx < 0 || idx >= line_count) return;
    free(lines[idx]);
    lines[idx] = strdup(text);
}

char *replace_spaces(const char *s) {
    char *r = strdup(s);
    for (char *p = r; *p; p++) if (*p == ' ') *p = '_';
    return r;
}

void free_lines(void) {
    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        int idx;
        char text[4096];
        int n = sscanf(line, "%s %d", cmd, &idx);
        if (strcmp(cmd, "INSERT") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1;
            while (*p == ' ') p++;
            char *end = p;
            while (*end && *end != ' ') end++;
            if (!*end) continue;
            char *rest = end + 1;
            while (*rest == ' ') rest++;
            if (*rest) insert_line(idx, rest);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (*p) append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1;
            while (*p == ' ') p++;
            char *end = p;
            while (*end && *end != ' ') end++;
            if (!*end) continue;
            char *rest = end + 1;
            while (*rest == ' ') rest++;
            if (*rest) patch_line(idx, rest);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < line_count; i++) {
                char *s = replace_spaces(lines[i]);
                printf("%s\n", s);
                free(s);
            }
        }
    }
    free_lines();
    return 0;
}