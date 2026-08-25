// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int line_count = 0;
int line_capacity = 0;

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    if (line_count + 1 > line_capacity) {
        line_capacity = line_capacity ? line_capacity * 2 : 4;
        char **new = realloc(lines, line_capacity * sizeof(char *));
        if (!new) return;
        lines = new;
    }
    for (int i = line_count; i > index; i--) {
        lines[i] = lines[i - 1];
    }
    lines[index] = strdup(text);
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    free(lines[index]);
    for (int i = index; i < line_count - 1; i++) {
        lines[i] = lines[i + 1];
    }
    line_count--;
}

void patch_line(int index, const char *text) {
    if (index < 0 || index >= line_count) return;
    free(lines[index]);
    lines[index] = strdup(text);
}

void print_lines(void) {
    for (int i = 0; i < line_count; i++) {
        char *s = lines[i];
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
}

void free_all(void) {
    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            int index;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) != 1) continue;
            char *txt = strchr(p, ' ');
            if (!txt) continue;
            while (*txt == ' ') txt++;
            insert_line(index, txt);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int index;
            if (sscanf(line + 7, "%d", &index) == 1) delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0) {
            int index;
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) != 1) continue;
            char *txt = strchr(p, ' ');
            if (!txt) continue;
            while (*txt == ' ') txt++;
            patch_line(index, txt);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    return 0;
}