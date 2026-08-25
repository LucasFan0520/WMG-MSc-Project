// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
size_t count = 0, cap = 0;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

void add_line(size_t idx, const char *text) {
    if (idx > count) idx = count;
    if (count == cap) {
        size_t new_cap = cap ? cap * 2 : 4;
        char **tmp = realloc(lines, new_cap * sizeof(char *));
        if (!tmp) return;
        lines = tmp; cap = new_cap;
    }
    for (size_t i = count; i > idx; i--) lines[i] = lines[i - 1];
    lines[idx] = strdup(text);
    count++;
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = get_token(&ptr);
            if (idx_str) {
                size_t idx = strtoul(idx_str, NULL, 10);
                add_line(idx, ptr);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            add_line(count, ptr);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_str = get_token(&ptr);
            if (idx_str) {
                size_t idx = strtoul(idx_str, NULL, 10);
                if (idx < count) {
                    free(lines[idx]);
                    for (size_t i = idx; i < count - 1; i++) lines[i] = lines[i + 1];
                    count--;
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_str = get_token(&ptr);
            if (idx_str) {
                size_t idx = strtoul(idx_str, NULL, 10);
                if (idx < count) {
                    char *tmp = strdup(ptr);
                    if (tmp) { free(lines[idx]); lines[idx] = tmp; }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (char *p = lines[i]; *p; p++) putchar(*p == ' ' ? '_' : *p);
                putchar('\n');
            }
        }
    }
    for (size_t i = 0; i < count; i++) free(lines[i]);
    free(lines);
    return 0;
}