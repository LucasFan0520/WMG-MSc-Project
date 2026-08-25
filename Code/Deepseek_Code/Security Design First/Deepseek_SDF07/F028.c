// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *stop;
    char *note;
} Stop;

typedef struct {
    Stop *items;
    size_t count;
    size_t capacity;
} Route;

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

static void route_init(Route *r) {
    r->items = NULL;
    r->count = 0;
    r->capacity = 0;
}

static void route_free(Route *r) {
    for (size_t i = 0; i < r->count; i++) {
        free(r->items[i].stop);
        free(r->items[i].note);
    }
    free(r->items);
    r->items = NULL;
    r->count = 0;
    r->capacity = 0;
}

static void route_add(Route *r, const char *stop, const char *note) {
    if (r->count == r->capacity) {
        size_t newcap = r->capacity ? r->capacity * 2 : 4;
        Stop *tmp = realloc(r->items, newcap * sizeof(Stop));
        if (!tmp) return;
        r->items = tmp;
        r->capacity = newcap;
    }
    r->items[r->count].stop = safe_strdup(stop);
    r->items[r->count].note = safe_strdup(note);
    if (r->items[r->count].stop && r->items[r->count].note)
        r->count++;
    else {
        free(r->items[r->count].stop);
        free(r->items[r->count].note);
    }
}

static void route_insert(Route *r, int idx, const char *stop, const char *note) {
    if (idx < 0 || idx > (int)r->count) return;
    if (r->count == r->capacity) {
        size_t newcap = r->capacity ? r->capacity * 2 : 4;
        Stop *tmp = realloc(r->items, newcap * sizeof(Stop));
        if (!tmp) return;
        r->items = tmp;
        r->capacity = newcap;
    }
    for (size_t i = r->count; i > (size_t)idx; i--)
        r->items[i] = r->items[i-1];
    r->items[idx].stop = safe_strdup(stop);
    r->items[idx].note = safe_strdup(note);
    if (r->items[idx].stop && r->items[idx].note)
        r->count++;
    else {
        free(r->items[idx].stop);
        free(r->items[idx].note);
    }
}

static void route_move(Route *r, int from, int to) {
    if (from < 0 || from >= (int)r->count || to < 0 || to >= (int)r->count) return;
    if (from == to) return;
    Stop tmp = r->items[from];
    if (from < to) {
        for (int i = from; i < to; i++)
            r->items[i] = r->items[i+1];
    } else {
        for (int i = from; i > to; i--)
            r->items[i] = r->items[i-1];
    }
    r->items[to] = tmp;
}

static void route_remove(Route *r, int idx) {
    if (idx < 0 || idx >= (int)r->count) return;
    free(r->items[idx].stop);
    free(r->items[idx].note);
    for (size_t i = idx; i < r->count - 1; i++)
        r->items[i] = r->items[i+1];
    r->count--;
}

static void route_find(Route *r, const char *stop) {
    for (size_t i = 0; i < r->count; i++) {
        if (strcmp(r->items[i].stop, stop) == 0) {
            const char *p = r->items[i].note;
            while (*p) {
                putchar(*p == ' ' ? '_' : *p);
                p++;
            }
            putchar('\n');
            return;
        }
    }
}

static void route_print(Route *r) {
    for (size_t i = 0; i < r->count; i++) {
        printf("%s ", r->items[i].stop);
        const char *p = r->items[i].note;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
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
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    Route r;
    route_init(&r);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[64], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "ADD") == 0) {
            route_add(&r, a1, a2);
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx = atoi(a1);
            route_insert(&r, idx, a2, a3);
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from = atoi(a1);
            int to = atoi(a2);
            route_move(&r, from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = atoi(a1);
            route_remove(&r, idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            route_find(&r, a1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            route_print(&r);
        }
    }
    route_free(&r);
    return 0;
}