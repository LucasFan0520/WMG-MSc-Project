/*
 * F009.c
 */
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

void add_event(const char *group, const char *desc) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group);
        g->descs = NULL;
        g->next = groups;
        groups = g;
    }
    Desc *d = malloc(sizeof(Desc));
    d->text = strdup(desc);
    d->next = g->descs;
    g->descs = d;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(newg)) return;
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newg);
    ng->descs = NULL;
    Desc *d = ga->descs;
    while (d) {
        Desc *nd = malloc(sizeof(Desc));
        nd->text = strdup(d->text);
        nd->next = ng->descs;
        ng->descs = nd;
        d = d->next;
    }
    d = gb->descs;
    while (d) {
        Desc *nd = malloc(sizeof(Desc));
        nd->text = strdup(d->text);
        nd->next = ng->descs;
        ng->descs = nd;
        d = d->next;
    }
    ng->next = groups;
    groups = ng;
}

void delete_group(const char *name) {
    Group *prev = NULL, *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            Desc *d = g->descs;
            while (d) {
                Desc *tmp = d;
                d = d->next;
                free(tmp->text);
                free(tmp);
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
    Group *g = find_group(name);
    int cnt = 0;
    if (g) {
        Desc *d = g->descs;
        while (d) { cnt++; d = d->next; }
    }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        Desc *d = g->descs;
        while (d) {
            printf("%s %s\n", g->name, d->text);
            d = d->next;
        }
        g = g->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3) < 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *rest = line + strlen(cmd) + strlen(arg1) + 1;
            while (*rest == ' ') rest++;
            add_event(arg1, rest);
        } else if (strcmp(cmd, "MERGE") == 0) {
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    return 0;
}