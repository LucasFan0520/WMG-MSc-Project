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
    for (Group *g = head; g; g = g->next) {
        if (strcmp(g->name, name) == 0) {
            return g;
        }
    }
    return NULL;
}

void add_event(const char *g, const char *d) {
    Group *grp = find_group(g);
    if (!grp) {
        grp = malloc(sizeof(Group));
        grp->name = strdup(g);
        grp->events = NULL;
        grp->next = head;
        head = grp;
    }
    Event *e = malloc(sizeof(Event));
    e->desc = strdup(d);
    e->next = grp->events;
    grp->events = e;
}

void merge_group(const char *a, const char *b, const char *n) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    Group *gn = find_group(n);
    if (!gn) {
        gn = malloc(sizeof(Group));
        gn->name = strdup(n);
        gn->events = NULL;
        gn->next = head;
        head = gn;
    }
    if (ga) {
        for (Event *e = ga->events; e; e = e->next) {
            Event *ne = malloc(sizeof(Event));
            ne->desc = strdup(e->desc);
            ne->next = gn->events;
            gn->events = ne;
        }
    }
    if (gb) {
        for (Event *e = gb->events; e; e = e->next) {
            Event *ne = malloc(sizeof(Event));
            ne->desc = strdup(e->desc);
            ne->next = gn->events;
            gn->events = ne;
        }
    }
}

void delete_group(const char *name) {
    Group *p = head;
    Group *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            Group *next = p->next;
            Event *e = p->events;
            while (e) {
                Event *en = e->next;
                free(e->desc);
                free(e);
                e = en;
            }
            free(p->name);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int c = 0;
    if (g) {
        for (Event *e = g->events; e; e = e->next) {
            c++;
        }
    }
    printf("%d\n", c);
}

void report_groups() {
    for (Group *g = head; g; g = g->next) {
        int c = 0;
        for (Event *e = g->events; e; e = e->next) {
            c++;
        }
        printf("%s %d\n", g->name, c);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char a[1000];
        char b[1000];
        char c[1000];
        b[0] = 0;
        c[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "EVENT") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            add_event(a, b);
        } else if (strcmp(cmd, "MERGE") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, c);
            merge_group(a, b, c);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            sscanf(line, "%*s %s", a);
            delete_group(a);
        } else if (strcmp(cmd, "COUNT") == 0) {
            sscanf(line, "%*s %s", a);
            count_group(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    while (head) {
        Group *n = head->next;
        Event *e = head->events;
        while (e) {
            Event *en = e->next;
            free(e->desc);
            free(e);
            e = en;
        }
        free(head->name);
        free(head);
        head = n;
    }
    return 0;
}