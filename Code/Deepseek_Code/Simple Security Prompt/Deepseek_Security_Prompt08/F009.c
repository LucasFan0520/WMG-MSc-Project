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

Group *head = NULL, *tail = NULL;

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
        g->next = NULL;
        if (tail) {
            tail->next = g;
            tail = g;
        } else {
            head = tail = g;
        }
    }
    Event *e = malloc(sizeof(Event));
    e->desc = malloc(strlen(desc) + 1);
    strcpy(e->desc, desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *gn = malloc(sizeof(Group));
    gn->name = malloc(strlen(newg) + 1);
    strcpy(gn->name, newg);
    gn->events = NULL;
    gn->next = NULL;
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
    if (tail) {
        tail->next = gn;
        tail = gn;
    } else {
        head = tail = gn;
    }
}

void delete_group(const char *name) {
    Group *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Event *e = curr->events;
            while (e) {
                Event *next = e->next;
                free(e->desc);
                free(e);
                e = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_events(const char *name) {
    Group *g = find_group(name);
    if (!g) { printf("0\n"); return; }
    int cnt = 0;
    Event *e = g->events;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = head;
    while (g) {
        printf("%s\n", g->name);
        g = g->next;
    }
}

void free_all(void) {
    Group *g = head;
    while (g) {
        Group *nextg = g->next;
        Event *e = g->events;
        while (e) {
            Event *nexte = e->next;
            free(e->desc);
            free(e);
            e = nexte;
        }
        free(g->name);
        free(g);
        g = nextg;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = strtok(NULL, " ");
            char *desc = strtok(NULL, "");
            if (group && desc) add_event(group, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *a = strtok(NULL, " ");
            char *b = strtok(NULL, " ");
            char *newg = strtok(NULL, " ");
            if (a && b && newg) merge_groups(a, b, newg);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_group(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) count_events(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free(line);
    free_all();
    return 0;
}