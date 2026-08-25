/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ev {
    char *desc;
    struct ev *next;
} ev_t;

typedef struct group {
    char *name;
    ev_t *events;
    struct group *next;
} group_t;

group_t *groups = NULL;

group_t *find_group(const char *name) {
    group_t *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) return g;
        g = g->next;
    }
    return NULL;
}

void add_event(const char *gname, const char *desc) {
    group_t *g = find_group(gname);
    if (!g) {
        g = malloc(sizeof(group_t));
        if (!g) return;
        g->name = strdup(gname);
        g->events = NULL;
        g->next = groups;
        groups = g;
        if (!g->name) { free(g); return; }
    }
    ev_t *e = malloc(sizeof(ev_t));
    if (!e) return;
    e->desc = strdup(desc);
    e->next = NULL;
    if (!e->desc) { free(e); return; }
    ev_t **last = &g->events;
    while (*last) last = &(*last)->next;
    *last = e;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    group_t *ga = find_group(a);
    group_t *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(newg)) return;
    group_t *g = malloc(sizeof(group_t));
    if (!g) return;
    g->name = strdup(newg);
    g->events = NULL;
    g->next = groups;
    groups = g;
    if (!g->name) { free(g); return; }
    // copy events from a
    ev_t *e = ga->events;
    while (e) {
        add_event(newg, e->desc);
        e = e->next;
    }
    e = gb->events;
    while (e) {
        add_event(newg, e->desc);
        e = e->next;
    }
}

void delete_group(const char *name) {
    group_t *prev = NULL, *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            ev_t *e = g->events;
            while (e) {
                ev_t *nxt = e->next;
                free(e->desc);
                free(e);
                e = nxt;
            }
            free(g->name);
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(const char *name) {
    group_t *g = find_group(name);
    if (!g) { printf("0\n"); return; }
    int n = 0;
    ev_t *e = g->events;
    while (e) { n++; e = e->next; }
    printf("%d\n", n);
}

void report_groups(void) {
    group_t *g = groups;
    while (g) {
        printf("%s\n", g->name);
        ev_t *e = g->events;
        while (e) {
            printf("  %s\n", e->desc);
            e = e->next;
        }
        g = g->next;
    }
}

void free_all(void) {
    group_t *g = groups;
    while (g) {
        group_t *nxt = g->next;
        ev_t *e = g->events;
        while (e) {
            ev_t *n = e->next;
            free(e->desc);
            free(e);
            e = n;
        }
        free(g->name);
        free(g);
        g = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[256], b[256], c[256];
        if (sscanf(line, "%19s %255s %255s %255s", cmd, a, b, c) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "EVENT") == 0) {
                add_event(a, rest);
            } else if (strcmp(cmd, "MERGE") == 0) {
                if (sscanf(line, "%*s %255s %255s %255s", a, b, c) == 3)
                    merge_groups(a, b, c);
            } else if (strcmp(cmd, "DELETEGROUP") == 0) {
                delete_group(a);
            } else if (strcmp(cmd, "COUNT") == 0) {
                count_group(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_groups();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}