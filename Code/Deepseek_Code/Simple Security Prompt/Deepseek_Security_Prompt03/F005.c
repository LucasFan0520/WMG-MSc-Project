/*
 * F005.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0, capacity = 0;

void ensure_capacity(int needed) {
    if (needed > capacity) {
        capacity = needed + 10;
        lines = realloc(lines, sizeof(char*) * capacity);
    }
}

void append_line(const char *text) {
    ensure_capacity(count + 1);
    lines[count++] = strdup(text);
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > count) return;
    ensure_capacity(count + 1);
    memmove(&lines[index+1], &lines[index], sizeof(char*) * (count - index));
    lines[index] = strdup(text);
    count++;
}

void delete_line(int index) {
    if (index < 0 || index >= count) return;
    free(lines[index]);
    memmove(&lines[index], &lines[index+1], sizeof(char*) * (count - index - 1));
    count--;
}

void patch_line(int index, const char *text) {
    if (index < 0 || index >= count) return;
    free(lines[index]);
    lines[index] = strdup(text);
}

void print_lines(void) {
    for (int i = 0; i < count; i++) {
        for (char *s = lines[i]; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char text[4096];
            if (sscanf(rest, "%d", &idx) == 1) {
                char *txt = rest;
                while (*txt == ' ') txt++;
                while (*txt >= '0' && *txt <= '9') txt++;
                while (*txt == ' ') txt++;
                insert_line(idx, txt);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_line(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            if (sscanf(rest, "%d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            if (sscanf(rest, "%d", &idx) == 1) {
                char *txt = rest;
                while (*txt == ' ') txt++;
                while (*txt >= '0' && *txt <= '9') txt++;
                while (*txt == ' ') txt++;
                patch_line(idx, txt);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    return 0;
}