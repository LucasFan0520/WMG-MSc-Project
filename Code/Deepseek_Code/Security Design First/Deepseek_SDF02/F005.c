/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0;
int capacity = 0;

void ensure_capacity(int needed) {
    if (needed <= capacity) return;
    int newcap = capacity ? capacity * 2 : 4;
    while (newcap < needed) newcap *= 2;
    char **new = realloc(lines, sizeof(char*) * newcap);
    if (!new) return;
    lines = new;
    capacity = newcap;
}

void insert_line(int idx, const char *text) {
    if (idx < 0 || idx > count) return;
    ensure_capacity(count + 1);
    if (!lines) return;
    memmove(&lines[idx+1], &lines[idx], (count - idx) * sizeof(char*));
    char *new = strdup(text);
    if (!new) return;
    lines[idx] = new;
    count++;
}

void append_line(const char *text) {
    insert_line(count, text);
}

void delete_line(int idx) {
    if (idx < 0 || idx >= count) return;
    free(lines[idx]);
    memmove(&lines[idx], &lines[idx+1], (count - idx - 1) * sizeof(char*));
    count--;
}

void patch_line(int idx, const char *text) {
    if (idx < 0 || idx >= count) return;
    char *new = strdup(text);
    if (!new) return;
    free(lines[idx]);
    lines[idx] = new;
}

void print_lines(void) {
    for (int i = 0; i < count; i++) {
        for (char *s = lines[i]; *s; s++) {
            if (*s == ' ') putchar('_');
            else putchar(*s);
        }
        putchar('\n');
    }
}

void free_all(void) {
    for (int i = 0; i < count; i++) free(lines[i]);
    free(lines);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10];
        int idx;
        if (sscanf(line, "%9s", cmd) != 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(rest, "%d", &idx) == 1) {
                const char *text = rest;
                while (*text && *text != ' ') text++;
                while (*text == ' ') text++;
                insert_line(idx, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_line(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%d", &idx) == 1)
                delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(rest, "%d", &idx) == 1) {
                const char *text = rest;
                while (*text && *text != ' ') text++;
                while (*text == ' ') text++;
                patch_line(idx, text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free(line);
    free_all();
    return 0;
}