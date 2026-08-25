// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char **lines;
    size_t count;
    size_t capacity;
} Editor;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void ed_init(Editor *e) {
    e->lines = NULL;
    e->count = 0;
    e->capacity = 0;
}

static void ed_free(Editor *e) {
    for (size_t i = 0; i < e->count; i++)
        free(e->lines[i]);
    free(e->lines);
    e->lines = NULL;
    e->count = 0;
    e->capacity = 0;
}

static void ed_insert(Editor *e, int idx, const char *text) {
    if (idx < 0 || idx > (int)e->count) return;
    if (e->count == e->capacity) {
        size_t newcap = e->capacity ? e->capacity * 2 : 4;
        char **tmp = realloc(e->lines, newcap * sizeof(char *));
        if (!tmp) return;
        e->lines = tmp;
        e->capacity = newcap;
    }
    for (size_t i = e->count; i > (size_t)idx; i--)
        e->lines[i] = e->lines[i-1];
    e->lines[idx] = safe_strdup(text);
    if (e->lines[idx]) e->count++;
}

static void ed_append(Editor *e, const char *text) {
    ed_insert(e, (int)e->count, text);
}

static void ed_delete(Editor *e, int idx) {
    if (idx < 0 || idx >= (int)e->count) return;
    free(e->lines[idx]);
    for (size_t i = idx; i < e->count - 1; i++)
        e->lines[i] = e->lines[i+1];
    e->count--;
}

static void ed_patch(Editor *e, int idx, const char *text) {
    if (idx < 0 || idx >= (int)e->count) return;
    char *new = safe_strdup(text);
    if (!new) return;
    free(e->lines[idx]);
    e->lines[idx] = new;
}

static void ed_print(Editor *e) {
    for (size_t i = 0; i < e->count; i++) {
        const char *p = e->lines[i];
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
}

int main(void) {
    Editor e;
    ed_init(&e);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[64], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "INSERT") == 0) {
            int idx = atoi(a1);
            ed_insert(&e, idx, a2);
        } else if (strcmp(cmd, "APPEND") == 0) {
            ed_append(&e, a1); // APPEND text, so a1 is text
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = atoi(a1);
            ed_delete(&e, idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx = atoi(a1);
            ed_patch(&e, idx, a2);
        } else if (strcmp(cmd, "PRINT") == 0) {
            ed_print(&e);
        }
    }
    ed_free(&e);
    return 0;
}