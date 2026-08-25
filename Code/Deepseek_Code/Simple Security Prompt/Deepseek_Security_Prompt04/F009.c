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

Group *find_group(char *name) {
    for (Group *g = head; g; g = g->next)
        if (strcmp(g->name, name) == 0) return g;
    return NULL;
}

void add_event(char *group, char *description) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group);
        g->events = NULL;
        g->next = head;
        head = g;
    }
    Event *e = malloc(sizeof(Event));
    e->description = strdup(description);
    e->next = g->events;
    g->events = e;
}

void merge_groups(char *a, char *b, char *newName) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *gn = malloc(sizeof(Group));
    gn->name = strdup(newName);
    gn->events = NULL;
    // Copy events from ga and gb (order not important)
    for (Event *e = ga->events; e; e = e->next) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = gn->events;
        gn->events = ne;
    }
    for (Event *e = gb->events; e; e = e->next) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = gn->events;
        gn->events = ne;
    }
    gn->next = head;
    head = gn;
}

void delete_group(char *name) {
    Group *prev = NULL;
    for (Group *g = head; g; g = g->next) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else head = g->next;
            free(g->name);
            Event *e = g->events;
            while (e) {
                Event *next = e->next;
                free(e->description);
                free(e);
                e = next;
            }
            free(g);
            return;
        }
        prev = g;
    }
}

void count_events(char *name) {
    Group *g = find_group(name);
    int cnt = 0;
    if (g) {
        for (Event *e = g->events; e; e = e->next) cnt++;
    }
    printf("%d\n", cnt);
}

void report_all(void) {
    for (Group *g = head; g; g = g->next) {
        printf("%s\n", g->name);
        for (Event *e = g->events; e; e = e->next)
            printf("  %s\n", e->description);
    }
}

void free_all(void) {
    Group *g = head;
    while (g) {
        Group *next = g->next;
        free(g->name);
        Event *e = g->events;
        while (e) {
            Event *enext = e->next;
            free(e->description);
            free(e);
            e = enext;
        }
        free(g);
        g = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "EVENT") == 0 && n == 3) {
            add_event(arg1, arg2);
        } else if (strcmp(cmd, "MERGE") == 0 && n == 4) {
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0 && n == 2) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0 && n == 2) {
            count_events(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}