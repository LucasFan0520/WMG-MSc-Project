// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Event {
    char *desc;
    struct Event *next;
} Event;

typedef struct Group {
    char *name;
    Event *events;
    struct Group *next;
} Group;

Group *head = NULL;

Group *find_group(const char *name) {
    Group *g = head;
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
        g->name = malloc(strlen(group) + 1);
        strcpy(g->name, group);
        g->events = NULL;
        g->next = head;
        head = g;
    }
    Event *e = malloc(sizeof(Event));
    e->desc = malloc(strlen(desc) + 1);
    strcpy(e->desc, desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *a, const char *b, const char *new) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(new)) return;
    Group *gn = malloc(sizeof(Group));
    gn->name = malloc(strlen(new) + 1);
    strcpy(gn->name, new);
    gn->events = NULL;
    Event *e;
    for (e = ga->events; e; e = e->next) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = malloc(strlen(e->desc) + 1);
        strcpy(ne->desc, e->desc);
        ne->next = gn->events;
        gn->events = ne;
    }
    for (e = gb->events; e; e = e->next) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = malloc(strlen(e->desc) + 1);
        strcpy(ne->desc, e->desc);
        ne->next = gn->events;
        gn->events = ne;
    }
    gn->next = head;
    head = gn;
}

void delete_group(const char *name) {
    Group *prev = NULL;
    Group *g = head;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else head = g->next;
            free(g->name);
            Event *e = g->events;
            while (e) {
                Event *next = e->next;
                free(e->desc);
                free(e);
                e = next;
            }
            free(g);
            return;
        }
        prev = g;
        g = g->next;
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
    Group *g = head;
    while (g) {
        printf("%s:", g->name);
        Event *e = g->events;
        int first = 1;
        while (e) {
            if (!first) putchar(' ');
            for (char *c = e->desc; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            first = 0;
            e = e->next;
        }
        putchar('\n');
        g = g->next;
    }
}

void free_all(void) {
    Group *g = head;
    while (g) {
        Group *nextg = g->next;
        free(g->name);
        Event *e = g->events;
        while (e) {
            Event *nexte = e->next;
            free(e->desc);
            free(e);
            e = nexte;
        }
        free(g);
        g = nextg;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_event(group, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *a = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *b = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            merge_groups(a, b, p);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            delete_group(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            count_group(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}