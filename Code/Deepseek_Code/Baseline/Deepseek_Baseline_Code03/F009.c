// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct event {
    char *desc;
    struct event *next;
};

struct group {
    char *name;
    struct event *events;
    struct group *next;
};

struct group *groups = NULL;
struct group *tail = NULL;

struct group *find_group(char *name) {
    struct group *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) return g;
        g = g->next;
    }
    return NULL;
}

void add_event(char *group, char *desc) {
    struct group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(struct group));
        g->name = malloc(strlen(group) + 1);
        strcpy(g->name, group);
        g->events = NULL;
        g->next = NULL;
        if (tail) {
            tail->next = g;
            tail = g;
        } else {
            groups = tail = g;
        }
    }
    struct event *e = malloc(sizeof(struct event));
    e->desc = malloc(strlen(desc) + 1);
    strcpy(e->desc, desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(char *a, char *b, char *newg) {
    struct group *ga = find_group(a);
    struct group *gb = find_group(b);
    if (!ga || !gb) return;
    struct group *ng = malloc(sizeof(struct group));
    ng->name = malloc(strlen(newg) + 1);
    strcpy(ng->name, newg);
    ng->events = NULL;
    // copy events from ga
    struct event *e = ga->events;
    while (e) {
        struct event *ne = malloc(sizeof(struct event));
        ne->desc = malloc(strlen(e->desc) + 1);
        strcpy(ne->desc, e->desc);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
    // copy from gb
    e = gb->events;
    while (e) {
        struct event *ne = malloc(sizeof(struct event));
        ne->desc = malloc(strlen(e->desc) + 1);
        strcpy(ne->desc, e->desc);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
    ng->next = NULL;
    if (tail) {
        tail->next = ng;
        tail = ng;
    } else {
        groups = tail = ng;
    }
}

void delete_group(char *name) {
    struct group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            if (tail == g) tail = prev;
            // free events
            struct event *e = g->events;
            while (e) {
                struct event *next = e->next;
                free(e->desc);
                free(e);
                e = next;
            }
            free(g->name);
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(char *name) {
    struct group *g = find_group(name);
    if (g) {
        int cnt = 0;
        struct event *e = g->events;
        while (e) { cnt++; e = e->next; }
        printf("%d\n", cnt);
    } else {
        printf("0\n");
    }
}

void report_groups() {
    struct group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        struct event *e = g->events;
        while (e) {
            printf("  %s\n", e->desc);
            e = e->next;
        }
        g = g->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "EVENT") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_event(arg1, rest);
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
    return 0;
}