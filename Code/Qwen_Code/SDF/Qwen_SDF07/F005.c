// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char **lines = NULL;
size_t lc = 0, lcap = 0;

void insert_l(size_t idx, const char *text) {
    if (idx > lc) return;
    if (lc == lcap) {
        size_t nc = lcap ? lcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(char*)) { fprintf(stderr, "Overflow\n"); exit(1); }
        char **tmp = realloc(lines, nc * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        lines = tmp;
        lcap = nc;
    }
    for (size_t i = lc; i > idx; i--) lines[i] = lines[i-1];
    lines[idx] = strdup(text);
    if (!lines[idx]) { perror("strdup"); exit(1); }
    lc++;
}

void delete_l(size_t idx) {
    if (idx >= lc) return;
    free(lines[idx]);
    for (size_t i = idx; i < lc - 1; i++) lines[i] = lines[i+1];
    lc--;
}

void patch_l(size_t idx, const char *text) {
    if (idx >= lc) return;
    char *tmp = strdup(text);
    if (!tmp) { perror("strdup"); exit(1); }
    free(lines[idx]);
    lines[idx] = tmp;
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "INSERT") == 0) {
            size_t idx;
            if (sscanf(line, "%*s %zu", &idx) != 1) { free(line); continue; }
            char *text = strchr(line, ' ');
            if (text) { text = strchr(text + 1, ' '); if (text) text++; }
            if (!text) text = "";
            insert_l(idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strchr(line, ' ');
            if (text) text++;
            if (!text) text = "";
            insert_l(lc, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            size_t idx;
            if (sscanf(line, "%*s %zu", &idx) == 1) delete_l(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            size_t idx;
            if (sscanf(line, "%*s %zu", &idx) != 1) { free(line); continue; }
            char *text = strchr(line, ' ');
            if (text) { text = strchr(text + 1, ' '); if (text) text++; }
            if (!text) text = "";
            patch_l(idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < lc; i++) {
                for (size_t j = 0; lines[i][j]; j++) putchar(lines[i][j] == ' ' ? '_' : lines[i][j]);
                putchar('\n');
            }
        }
        free(line);
    }
    for (size_t i = 0; i < lc; i++) free(lines[i]);
    free(lines);
    return 0;
}