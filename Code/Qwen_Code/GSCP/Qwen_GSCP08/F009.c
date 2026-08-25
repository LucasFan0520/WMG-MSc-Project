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

Group *find_group(char *name) {
    for (Group *g = head; g; g = g->next) {
        if (strcmp(g->name, name) == 0) return g;
    }
    return NULL;
}

void add_event(char *group, char *desc) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group);
        g->events = NULL;
        g->next = head;
        head = g;
    }
    Event *e = malloc(sizeof(Event));
    e->desc = strdup(desc);
    e->next = NULL;
    if (!g->events) {
        g->events = e;
    } else {
        Event *last = g->events;
        while (last->next) last = last->next;
        last->next = e;
    }
}

void merge_groups(char *a, char *b, char *new_name) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *gn = find_group(new_name);
    if (!gn) {
        gn = malloc(sizeof(Group));
        gn->name = strdup(new_name);
        gn->events = NULL;
        gn->next = head;
        head = gn;
    }
    for (Event *e = ga->events; e; e = e->next) add_event(new_name, e->desc);
    for (Event *e = gb->events; e; e = e->next) add_event(new_name, e->desc);
}

void delete_group(char *name) {
    Group *prev = NULL;
    Group *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Event *e = curr->events;
            while (e) {
                Event *next = e->next;
                free(e->desc);
                free(e);
                e = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_group(char *name) {
    Group *g = find_group(name);
    int count = 0;
    if (g) {
        for (Event *e = g->events; e; e = e->next) count++;
    }
    printf("%d\n", count);
}

void report_groups() {
    for (Group *g = head; g; g = g->next) {
        int count = 0;
        for (Event *e = g->events; e; e = e->next) count++;
        printf("%s %d\n", g->name, count);
    }
}

int main() {
    char cmd[20], a[100], b[100], desc[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "EVENT") == 0) {
            scanf("%99s", a);
            fgets(desc, sizeof(desc), stdin);
            while (desc[0] == ' ') memmove(desc, desc + 1, strlen(desc));
            desc[strcspn(desc, "\n")] = 0;
            add_event(a, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%99s%99s%99s", a, b, desc);
            merge_groups(a, b, desc);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            scanf("%99s", a);
            delete_group(a);
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%99s", a);
            count_group(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    return 0;
}