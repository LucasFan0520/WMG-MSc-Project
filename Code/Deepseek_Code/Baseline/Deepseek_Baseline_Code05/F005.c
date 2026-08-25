/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int size = 0, capacity = 0;

void ensure_capacity() {
    if (size >= capacity) {
        capacity = capacity ? capacity * 2 : 4;
        lines = realloc(lines, capacity * sizeof(char *));
    }
}

void insert_line(int idx, const char *text) {
    if (idx < 0 || idx > size) return;
    ensure_capacity();
    for (int i = size; i > idx; i--) lines[i] = lines[i-1];
    lines[idx] = strdup(text);
    size++;
}

void append_line(const char *text) {
    insert_line(size, text);
}

void delete_line(int idx) {
    if (idx < 0 || idx >= size) return;
    free(lines[idx]);
    for (int i = idx; i < size - 1; i++) lines[i] = lines[i+1];
    size--;
}

void patch_line(int idx, const char *text) {
    if (idx < 0 || idx >= size) return;
    free(lines[idx]);
    lines[idx] = strdup(text);
}

void print_lines() {
    for (int i = 0; i < size; i++) {
        for (char *c = lines[i]; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], text[1024];
        int idx;
        if (strcmp(line, "PRINT") == 0) { print_lines(); continue; }
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "APPEND") == 0) {
            char *rest = line + strlen(cmd) + 1;
            if (*rest == ' ') rest++;
            append_line(rest);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1) {
                char *rest = line + strlen(cmd) + 1 + (idx >= 0 ? snprintf(NULL,0,"%d",idx) : 0) + 1;
                if (*rest == ' ') rest++;
                insert_line(idx, rest);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1) {
                char *rest = line + strlen(cmd) + 1 + (idx >= 0 ? snprintf(NULL,0,"%d",idx) : 0) + 1;
                if (*rest == ' ') rest++;
                patch_line(idx, rest);
            }
        }
    }
    return 0;
}