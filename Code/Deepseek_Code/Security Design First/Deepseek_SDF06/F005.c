// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **lines = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void insert_line(size_t index, const char *text) {
    if (index > count) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        char **new = realloc(lines, capacity * sizeof(char *));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        lines = new;
    }
    for (size_t i = count; i > index; i--)
        lines[i] = lines[i - 1];
    lines[index] = strdup(text);
    if (!lines[index]) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void append_line(const char *text) {
    insert_line(count, text);
}

static void delete_line(size_t index) {
    if (index >= count) return;
    free(lines[index]);
    for (size_t i = index; i < count - 1; i++)
        lines[i] = lines[i + 1];
    count--;
}

static void patch_line(size_t index, const char *text) {
    if (index >= count) return;
    free(lines[index]);
    lines[index] = strdup(text);
    if (!lines[index]) { fprintf(stderr, "Memory error\n"); exit(1); }
}

static void print_lines(void) {
    for (size_t i = 0; i < count; i++) {
        for (char *p = lines[i]; *p; p++)
            putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
    }
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++)
        free(lines[i]);
    free(lines);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], text[4096];
    size_t idx;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %zu %4095[^\n]", &idx, text) == 2)
                insert_line(idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", text) == 1)
                append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %zu", &idx) == 1)
                delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(line, "%*s %zu %4095[^\n]", &idx, text) == 2)
                patch_line(idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }

    free(line);
    free_all();
    return 0;
}