// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char **lines; int count; int cap; } Lines;
Lines L = {0};

void ensure_cap(int n) {
    if (n > L.cap) {
        int newcap = L.cap ? L.cap * 2 : 16;
        while (newcap < n) newcap *= 2;
        char **tmp = realloc(L.lines, newcap * sizeof(char *));
        if (!tmp) return;
        L.lines = tmp; L.cap = newcap;
    }
}

void insert_line(int idx, const char *text) {
    if (idx < 0 || idx > L.count) return;
    ensure_cap(L.count + 1);
    if (!L.lines) return;
    for (int i = L.count; i > idx; i--) L.lines[i] = L.lines[i-1];
    L.lines[idx] = strdup(text);
    L.count++;
}

void append_line(const char *text) { insert_line(L.count, text); }

void delete_line(int idx) {
    if (idx < 0 || idx >= L.count) return;
    free(L.lines[idx]);
    for (int i = idx; i < L.count - 1; i++) L.lines[i] = L.lines[i+1];
    L.count--;
}

void patch_line(int idx, const char *text) {
    if (idx < 0 || idx >= L.count) return;
    free(L.lines[idx]);
    L.lines[idx] = strdup(text);
}

void print_lines(void) {
    for (int i = 0; i < L.count; i++) {
        for (char *c = L.lines[i]; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx; char text[4096];
            if (sscanf(rest, "%d", &idx) == 1) {
                char *t = rest;
                while (*t == ' ') t++;
                while (*t >= '0' && *t <= '9') t++;
                while (*t == ' ') t++;
                strcpy(text, t);
                insert_line(idx, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_line(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx; if (sscanf(rest, "%d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx; char text[4096];
            if (sscanf(rest, "%d", &idx) == 1) {
                char *t = rest;
                while (*t == ' ') t++;
                while (*t >= '0' && *t <= '9') t++;
                while (*t == ' ') t++;
                strcpy(text, t);
                patch_line(idx, text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    for (int i = 0; i < L.count; i++) free(L.lines[i]);
    free(L.lines);
    return 0;
}