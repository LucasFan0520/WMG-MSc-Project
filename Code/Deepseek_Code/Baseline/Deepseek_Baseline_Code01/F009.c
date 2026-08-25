// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct desc {
    char *text;
    struct desc *next;
} Desc;

typedef struct group {
    char *name;
    Desc *descs;
    struct group *next;
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

void add_desc(Group *g, const char *text) {
    Desc *d = malloc(sizeof(Desc));
    d->text = strdup(text);
    d->next = g->descs;
    g->descs = d;
}

void event_group(const char *name, const char *desc) {
    Group *g = find_group(name);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(name);
        g->descs = NULL;
        g->next = groups;
        groups = g;
    }
    add_desc(g, desc);
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a), *gb = find_group(b);
    if (!ga || !gb || find_group(newg)) return;
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newg);
    ng->descs = NULL;
    Desc *d = ga->descs;
    while (d) { add_desc(ng, d->text); d = d->next; }
    d = gb->descs;
    while (d) { add_desc(ng, d->text); d = d->next; }
    ng->next = groups;
    groups = ng;
}

void delete_group(const char *name) {
    Group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            Desc *d = g->descs;
            while (d) { Desc *tmp = d; d = d->next; free(tmp->text); free(tmp); }
            free(g->name);
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int cnt = 0;
    Desc *d = g ? g->descs : NULL;
    while (d) { cnt++; d = d->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        g = g->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                event_group(arg1, arg2);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", arg1, arg2, arg3) == 3)
                merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    return 0;
}