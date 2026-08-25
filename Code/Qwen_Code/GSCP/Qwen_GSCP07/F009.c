// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Desc { char *text; struct Desc *next; } Desc;
typedef struct Group { char *name; Desc *descs; struct Group *next; } Group;
Group *head = NULL;

Group *find_group(const char *name) {
    for (Group *g = head; g; g = g->next) if (strcmp(g->name, name) == 0) return g;
    return NULL;
}

void add_desc(Group *g, const char *text) {
    Desc *d = malloc(sizeof(Desc));
    d->text = strdup(text);
    d->next = g->descs;
    g->descs = d;
}

void event(const char *group, const char *desc) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group);
        g->descs = NULL;
        g->next = head;
        head = g;
    }
    add_desc(g, desc);
}

void merge(const char *a, const char *b, const char *c) {
    Group *ga = find_group(a), *gb = find_group(b);
    Group *gc = find_group(c);
    if (!gc) {
        gc = malloc(sizeof(Group));
        gc->name = strdup(c);
        gc->descs = NULL;
        gc->next = head;
        head = gc;
    }
    if (ga) for (Desc *d = ga->descs; d; d = d->next) add_desc(gc, d->text);
    if (gb) for (Desc *d = gb->descs; d; d = d->next) add_desc(gc, d->text);
}

void delete_group(const char *name) {
    Group **p = &head;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Group *tmp = *p;
            *p = tmp->next;
            while (tmp->descs) {
                Desc *d = tmp->descs->next;
                free(tmp->descs->text);
                free(tmp->descs);
                tmp->descs = d;
            }
            free(tmp->name);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int c = 0;
    if (g) for (Desc *d = g->descs; d; d = d->next) c++;
    printf("%d\n", c);
}

void report() {
    for (Group *g = head; g; g = g->next) {
        int c = 0;
        for (Desc *d = g->descs; d; d = d->next) c++;
        printf("%s %d\n", g->name, c);
    }
}

void cleanup() {
    while (head) {
        Group *tmp = head->next;
        while (head->descs) {
            Desc *d = head->descs->next;
            free(head->descs->text);
            free(head->descs);
            head->descs = d;
        }
        free(head->name);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) event(a1, t + 1); }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", a1, a2, a3) == 3) merge(a1, a2, a3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_group(a1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) count_group(a1);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    cleanup();
    return 0;
}