// filename: F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **lines = NULL;
static size_t lcount = 0;
static size_t lcap = 0;

static void cleanup(void) {
    for (size_t i = 0; i < lcount; ++i) free(lines[i]);
    free(lines);
}

static void insert_line(int index, const char *text) {
    if (index < 0 || index > (int)lcount) return;
    if (lcount == lcap) {
        size_t newcap = lcap ? lcap * 2 : 4;
        char **nl = realloc(lines, newcap * sizeof(char *));
        if (!nl) return;
        lines = nl;
        lcap = newcap;
    }
    for (size_t i = lcount; i > (size_t)index; --i)
        lines[i] = lines[i - 1];
    lines[index] = strdup(text);
    if (!lines[index]) return;
    ++lcount;
}

static void append_line(const char *text) {
    insert_line((int)lcount, text);
}

static void delete_line(int index) {
    if (index < 0 || index >= (int)lcount) return;
    free(lines[index]);
    for (size_t i = index; i + 1 < lcount; ++i)
        lines[i] = lines[i + 1];
    --lcount;
}

static void patch_line(int index, const char *text) {
    if (index < 0 || index >= (int)lcount) return;
    char *new = strdup(text);
    if (!new) return;
    free(lines[index]);
    lines[index] = new;
}

static void print_lines(void) {
    for (size_t i = 0; i < lcount; ++i) {
        for (char *p = lines[i]; *p; ++p)
            putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
    }
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        int idx = atoi(arg1);
        if (strcmp(cmd, "INSERT") == 0) {
            if (*arg1) insert_line(idx, arg2);
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_line(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (*arg1) patch_line(idx, arg2);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free(line);
    return 0;
}