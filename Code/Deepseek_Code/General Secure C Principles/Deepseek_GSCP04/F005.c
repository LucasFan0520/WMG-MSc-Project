/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **lines = NULL;
static size_t lcount = 0, lcap = 0;

static void insert_line(size_t idx, const char *text) {
    if (idx > lcount) return; // invalid
    if (lcount == lcap) {
        lcap = lcap ? lcap * 2 : 4;
        lines = realloc(lines, lcap * sizeof(char *));
        if (!lines) { perror("realloc"); exit(1); }
    }
    for (size_t i = lcount; i > idx; i--)
        lines[i] = lines[i - 1];
    lines[idx] = strdup(text);
    lcount++;
}

static void append_line(const char *text) {
    insert_line(lcount, text);
}

static void delete_line(size_t idx) {
    if (idx >= lcount) return;
    free(lines[idx]);
    for (size_t i = idx; i < lcount - 1; i++)
        lines[i] = lines[i + 1];
    lcount--;
}

static void patch_line(size_t idx, const char *text) {
    if (idx >= lcount) return;
    free(lines[idx]);
    lines[idx] = strdup(text);
}

static void print_lines(void) {
    for (size_t i = 0; i < lcount; i++) {
        for (char *c = lines[i]; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            char *idxstr = strtok(NULL, " ");
            if (!idxstr) continue;
            int idx = atoi(idxstr);
            if (idx < 0) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            insert_line((size_t)idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strtok(NULL, "");
            if (!text) text = "";
            append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idxstr = strtok(NULL, " ");
            if (!idxstr) continue;
            int idx = atoi(idxstr);
            if (idx < 0) continue;
            delete_line((size_t)idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idxstr = strtok(NULL, " ");
            if (!idxstr) continue;
            int idx = atoi(idxstr);
            if (idx < 0) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            patch_line((size_t)idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    for (size_t i = 0; i < lcount; i++)
        free(lines[i]);
    free(lines);
    return 0;
}