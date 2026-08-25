// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0;
int capacity = 0;

void ensure_capacity(int needed) {
    if (needed <= capacity) return;
    capacity = needed + 10;
    lines = realloc(lines, capacity * sizeof(char *));
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > count) return;
    ensure_capacity(count + 1);
    for (int i = count; i > index; i--) {
        lines[i] = lines[i-1];
    }
    lines[index] = strdup(text);
    count++;
}

void append_line(const char *text) {
    insert_line(count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= count) return;
    free(lines[index]);
    for (int i = index; i < count - 1; i++) {
        lines[i] = lines[i+1];
    }
    count--;
}

void patch_line(int index, const char *text) {
    if (index < 0 || index >= count) return;
    free(lines[index]);
    lines[index] = strdup(text);
}

void print_lines(void) {
    for (int i = 0; i < count; i++) {
        for (char *p = lines[i]; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
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
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "INSERT") == 0) {
            int index;
            char text[1024];
            if (sscanf(line, "%*s %d %1023[^\n]", &index, text) == 2) {
                insert_line(index, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char text[1024];
            if (sscanf(line, "%*s %1023[^\n]", text) == 1) {
                append_line(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int index;
            if (sscanf(line, "%*s %d", &index) == 1) {
                delete_line(index);
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            int index;
            char text[1024];
            if (sscanf(line, "%*s %d %1023[^\n]", &index, text) == 2) {
                patch_line(index, text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }

    free(line);
    free_all();
    return 0;
}