/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Event {
    char *description;
    struct Event *next;
} Event;

typedef struct Group {
    char *name;
    Event *events;
    struct Group *next;
} Group;

Group *head = NULL;

Group* find_group(const char *name) {
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
        g->name = strdup(group);
        g->events = NULL;
        g->next = head;
        head = g;
    }
    Event *e = malloc(sizeof(Event));
    e->description = strdup(desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *groupA, const char *groupB, const char *newGroup) {
    Group *a = find_group(groupA);
    Group *b = find_group(groupB);
    if (!a || !b) return;
    Group *ng = find_group(newGroup);
    if (ng) return;
    ng = malloc(sizeof(Group));
    ng->name = strdup(newGroup);
    ng->events = NULL;
    ng->next = head;
    head = ng;
    // copy events from a
    Event *e = a->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
    e = b->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
}

void delete_group(const char *name) {
    Group *g = head, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else head = g->next;
            Event *e = g->events;
            while (e) {
                Event *tmp = e;
                e = e->next;
                free(tmp->description);
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
    if (!g) { printf("0\n"); return; }
    int count = 0;
    Event *e = g->events;
    while (e) { count++; e = e->next; }
    printf("%d\n", count);
}

void report_groups() {
    Group *g = head;
    while (g) {
        printf("%s ", g->name);
        Event *e = g->events;
        while (e) {
            printf("%s", e->description);
            if (e->next) printf(" ");
            e = e->next;
        }
        printf("\n");
        g = g->next;
    }
}

void free_all() {
    while (head) {
        Group *tmp = head;
        head = head->next;
        Event *e = tmp->events;
        while (e) {
            Event *etmp = e;
            e = e->next;
            free(etmp->description);
            free(etmp);
        }
        free(tmp->name);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char a[256], b[256], c[256], desc[4096];
        int n = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, a, b, c, desc);
        if (strcmp(cmd, "EVENT") == 0 && n >= 3) {
            add_event(a, b);
        } else if (strcmp(cmd, "MERGE") == 0 && n == 4) {
            merge_groups(a, b, c);
        } else if (strcmp(cmd, "DELETEGROUP") == 0 && n == 2) {
            delete_group(a);
        } else if (strcmp(cmd, "COUNT") == 0 && n == 2) {
            count_group(a);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_groups();
        }
    }
    free_all();
    return 0;
}