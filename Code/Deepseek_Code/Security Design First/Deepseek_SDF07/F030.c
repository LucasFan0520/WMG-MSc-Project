// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *old;
    char *new;
} Rule;

typedef struct {
    Rule *items;
    size_t count;
    size_t capacity;
} RenameTable;

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

static void rt_init(RenameTable *rt) {
    rt->items = NULL;
    rt->count = 0;
    rt->capacity = 0;
}

static void rt_free(RenameTable *rt) {
    for (size_t i = 0; i < rt->count; i++) {
        free(rt->items[i].old);
        free(rt->items[i].new);
    }
    free(rt->items);
    rt->items = NULL;
    rt->count = 0;
    rt->capacity = 0;
}

static int rt_find(RenameTable *rt, const char *old) {
    for (size_t i = 0; i < rt->count; i++) {
        if (strcmp(rt->items[i].old, old) == 0)
            return (int)i;
    }
    return -1;
}

static void rt_add(RenameTable *rt, const char *old, const char *new) {
    if (rt_find(rt, old) >= 0) return;
    if (rt->count == rt->capacity) {
        size_t newcap = rt->capacity ? rt->capacity * 2 : 4;
        Rule *tmp = realloc(rt->items, newcap * sizeof(Rule));
        if (!tmp) return;
        rt->items = tmp;
        rt->capacity = newcap;
    }
    rt->items[rt->count].old = safe_strdup(old);
    rt->items[rt->count].new = safe_strdup(new);
    if (rt->items[rt->count].old && rt->items[rt->count].new)
        rt->count++;
    else {
        free(rt->items[rt->count].old);
        free(rt->items[rt->count].new);
    }
}

static void rt_apply(RenameTable *rt, const char *text) {
    for (size_t i = 0; i < rt->count; i++) {
        const char *pos = strstr(text, rt->items[i].old);
        if (pos) {
            printf("%s\n", rt->items[i].new);
            return;
        }
    }
    printf("%s\n", text);
}

static void rt_delete(RenameTable *rt, const char *old) {
    int idx = rt_find(rt, old);
    if (idx < 0) return;
    free(rt->items[idx].old);
    free(rt->items[idx].new);
    for (size_t i = idx; i < rt->count - 1; i++)
        rt->items[i] = rt->items[i+1];
    rt->count--;
}

static void rt_swap(RenameTable *rt, int i, int j) {
    if (i < 0 || i >= (int)rt->count || j < 0 || j >= (int)rt->count) return;
    Rule tmp = rt->items[i];
    rt->items[i] = rt->items[j];
    rt->items[j] = tmp;
}

static void rt_rules(RenameTable *rt) {
    for (size_t i = 0; i < rt->count; i++) {
        printf("%s %s\n", rt->items[i].old, rt->items[i].new);
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
    RenameTable rt;
    rt_init(&rt);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "ADD") == 0) {
            rt_add(&rt, a1, a2);
        } else if (strcmp(cmd, "APPLY") == 0) {
            rt_apply(&rt, a1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            rt_delete(&rt, a1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i = atoi(a1);
            int j = atoi(a2);
            rt_swap(&rt, i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            rt_rules(&rt);
        }
    }
    rt_free(&rt);
    return 0;
}