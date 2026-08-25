// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Event {
    char *description;
    struct Event *next;
} Event;

typedef struct Group {
    char *name;
    Event *events;
    struct Group *next;
} Group;

Group *groups = NULL;

Group *find_group(const char *name) {
    Group *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) return g;
        g = g->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_event(const char *group_name, const char *desc) {
    Group *g = find_group(group_name);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) return;
        g->name = safe_strdup(group_name);
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    Event *e = malloc(sizeof(Event));
    if (!e) return;
    e->description = safe_strdup(desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *a, const char *b, const char *new_name) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *ng = malloc(sizeof(Group));
    if (!ng) return;
    ng->name = safe_strdup(new_name);
    ng->events = NULL;
    Event *e = ga->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        if (ne) {
            ne->description = safe_strdup(e->description);
            ne->next = ng->events;
            ng->events = ne;
        }
        e = e->next;
    }
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        if (ne) {
            ne->description = safe_strdup(e->description);
            ne->next = ng->events;
            ng->events = ne;
        }
        e = e->next;
    }
    ng->next = groups;
    groups = ng;
}

void delete_group(const char *name) {
    Group *prev = NULL, *cur = groups;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else groups = cur->next;
            free(cur->name);
            Event *e = cur->events;
            while (e) {
                Event *next = e->next;
                free(e->description);
                free(e);
                e = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    if (!g) {
        printf("0\n");
        return;
    }
    int cnt = 0;
    Event *e = g->events;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        Event *e = g->events;
        while (e) {
            printf("%s %s\n", g->name, e->description);
            e = e->next;
        }
        g = g->next;
    }
}

void free_all(void) {
    Group *g = groups;
    while (g) {
        Group *next_g = g->next;
        free(g->name);
        Event *e = g->events;
        while (e) {
            Event *next_e = e->next;
            free(e->description);
            free(e);
            e = next_e;
        }
        free(g);
        g = next_g;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512];
        int n = sscanf(line, "%31s %511s %511s %511s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;

        if (strcmp(cmd, "EVENT") == 0 && n >= 3) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            add_event(arg1, p);
        } else if (strcmp(cmd, "MERGE") == 0 && n >= 4) {
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0 && n >= 2) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}