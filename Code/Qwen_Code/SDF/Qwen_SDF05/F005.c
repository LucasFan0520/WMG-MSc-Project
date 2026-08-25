// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static char **lines = NULL;
static size_t lcnt = 0, lcap = 0;

static int lgrow(void) {
    if (lcnt < lcap) return 0;
    size_t nc = lcap ? lcap * 2 : 8;
    if (nc < lcap) return -1;
    char **t = realloc(lines, nc * sizeof(char *));
    if (!t) return -1;
    lines = t; lcap = nc;
    return 0;
}

static void cmd_insert(size_t idx, char *text) {
    if (idx > lcnt) return;
    if (lgrow() < 0) return;
    for (size_t i = lcnt; i > idx; i--) lines[i] = lines[i - 1];
    lines[idx] = safe_strdup(text);
    if (!lines[idx]) {
        for (size_t i = idx; i < lcnt; i--) lines[i] = lines[i + 1];
        return;
    }
    lcnt++;
}

static void cmd_append(char *text) {
    if (lgrow() < 0) return;
    lines[lcnt] = safe_strdup(text);
    if (!lines[lcnt]) return;
    lcnt++;
}

static void cmd_delete(size_t idx) {
    if (idx >= lcnt) return;
    free(lines[idx]);
    for (size_t i = idx; i + 1 < lcnt; i++) lines[i] = lines[i + 1];
    lcnt--;
}

static void cmd_patch(size_t idx, char *text) {
    if (idx >= lcnt) return;
    char *nt = safe_strdup(text);
    if (!nt) return;
    free(lines[idx]);
    lines[idx] = nt;
}

static void cmd_print(void) {
    for (size_t i = 0; i < lcnt; i++) {
        for (char *p = lines[i]; *p; p++)
            putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < lcnt; i++) free(lines[i]);
    free(lines); lines = NULL; lcnt = 0; lcap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "INSERT") == 0 || strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "PATCH") == 0) {
            int idx_val = 0; int n2 = 0;
            if (sscanf(rest, "%d%n", &idx_val, &n2) < 1) continue;
            if (idx_val < 0) continue;
            size_t idx = (size_t)idx_val;
            if (strcmp(cmd, "DELETE") == 0) {
                cmd_delete(idx);
            } else {
                char *text = rest + n2;
                while (*text == ' ') text++;
                if (strcmp(cmd, "INSERT") == 0) cmd_insert(idx, text);
                else cmd_patch(idx, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            cmd_append(rest);
        } else if (strcmp(cmd, "PRINT") == 0) {
            cmd_print();
        }
    }
    cleanup();
    return 0;
}