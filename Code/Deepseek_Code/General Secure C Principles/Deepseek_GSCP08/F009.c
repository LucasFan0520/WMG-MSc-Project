// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Desc { char *text; struct Desc *next; } Desc;
typedef struct Group { char *name; Desc *descs; struct Group *next; } Group;
Group *groups = NULL;

Group *find_group(const char *name) {
    for (Group *g = groups; g; g = g->next)
        if (strcmp(g->name, name) == 0) return g;
    return NULL;
}

void event_group(const char *name, const char *desc) {
    Group *g = find_group(name);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) return;
        g->name = strdup(name); g->descs = NULL; g->next = groups; groups = g;
    }
    Desc *d = malloc(sizeof(Desc));
    if (!d) return;
    d->text = strdup(desc);
    d->next = g->descs;
    g->descs = d;
}

void merge_groups(const char *a, const char *b, const char *ng) {
    Group *ga = find_group(a), *gb = find_group(b);
    if (!ga || !gb || find_group(ng)) return;
    Group *g = malloc(sizeof(Group));
    if (!g) return;
    g->name = strdup(ng); g->descs = NULL; g->next = groups; groups = g;
    for (Desc *d = ga->descs; d; d = d->next) {
        Desc *nd = malloc(sizeof(Desc));
        if (!nd) continue;
        nd->text = strdup(d->text);
        nd->next = g->descs;
        g->descs = nd;
    }
    for (Desc *d = gb->descs; d; d = d->next) {
        Desc *nd = malloc(sizeof(Desc));
        if (!nd) continue;
        nd->text = strdup(d->text);
        nd->next = g->descs;
        g->descs = nd;
    }
}

void delete_group(const char *name) {
    Group *prev = NULL;
    for (Group *g = groups; g; prev = g, g = g->next) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next; else groups = g->next;
            while (g->descs) { Desc *d = g->descs; g->descs = d->next; free(d->text); free(d); }
            free(g->name); free(g);
            return;
        }
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int cnt = 0;
    if (g) for (Desc *d = g->descs; d; d = d->next) cnt++;
    printf("%d\n", cnt);
}

void report_groups(void) {
    for (Group *g = groups; g; g = g->next) {
        printf("%s\n", g->name);
        for (Desc *d = g->descs; d; d = d->next) printf("  %s\n", d->text);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "EVENT") == 0) {
            char name[256], desc[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *d = rest + strlen(name);
                while (*d == ' ') d++;
                strcpy(desc, d);
                event_group(name, desc);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char a[256], b[256], ng[256];
            if (sscanf(rest, "%255s %255s %255s", a, b, ng) == 3) merge_groups(a, b, ng);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_group(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) count_group(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    while (groups) { Group *g = groups; groups = g->next; while (g->descs) { Desc *d = g->descs; g->descs = d->next; free(d->text); free(d); } free(g->name); free(g); }
    return 0;
}