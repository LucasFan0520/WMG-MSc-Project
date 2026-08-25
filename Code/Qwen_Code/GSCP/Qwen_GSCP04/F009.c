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
    Event *tail;
    int count;
    struct Group *next;
} Group;

Group *groups = NULL;

Group *find_group(const char *name) {
    Group *curr = groups;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_event(const char *group_name, const char *desc) {
    Group *g = find_group(group_name);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group_name);
        g->events = NULL;
        g->tail = NULL;
        g->count = 0;
        g->next = groups;
        groups = g;
    }
    Event *e = malloc(sizeof(Event));
    e->desc = strdup(desc);
    e->next = NULL;
    if (g->tail) g->tail->next = e;
    else g->events = e;
    g->tail = e;
    g->count++;
}

void merge_groups(const char *a, const char *b, const char *new_name) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *gn = find_group(new_name);
    if (!gn) {
        gn = malloc(sizeof(Group));
        gn->name = strdup(new_name);
        gn->events = NULL;
        gn->tail = NULL;
        gn->count = 0;
        gn->next = groups;
        groups = gn;
    }
    Event *curr = ga->events;
    while (curr) {
        Event *e = malloc(sizeof(Event));
        e->desc = strdup(curr->desc);
        e->next = NULL;
        if (gn->tail) gn->tail->next = e;
        else gn->events = e;
        gn->tail = e;
        gn->count++;
        curr = curr->next;
    }
    curr = gb->events;
    while (curr) {
        Event *e = malloc(sizeof(Event));
        e->desc = strdup(curr->desc);
        e->next = NULL;
        if (gn->tail) gn->tail->next = e;
        else gn->events = e;
        gn->tail = e;
        gn->count++;
        curr = curr->next;
    }
}

void delete_group(const char *name) {
    Group *curr = groups, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else groups = curr->next;
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

void count_group(const char *name) {
    Group *g = find_group(name);
    printf("%d\n", g ? g->count : 0);
}

void report_groups() {
    Group *curr = groups;
    while (curr) {
        printf("%s %d\n", curr->name, curr->count);
        curr = curr->next;
    }
}

void free_all() {
    Group *curr = groups;
    while (curr) {
        Group *next = curr->next;
        Event *e = curr->events;
        while (e) {
            Event *next_e = e->next;
            free(e->desc);
            free(e);
            e = next_e;
        }
        free(curr->name);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], arg1[256], arg2[256], arg3[256], desc[4096] = {0};
        int n = sscanf(line, "%31s %255s %255s %255s %4095[^\n]", cmd, arg1, arg2, arg3, desc);
        
        if (strcmp(cmd, "EVENT") == 0 && n >= 2) {
            char *d = strchr(line + strlen(cmd) + 1 + strlen(arg1) + 1, ' ');
            if (d) add_event(arg1, d + 1);
        }
        else if (strcmp(cmd, "MERGE") == 0 && n >= 3) merge_groups(arg1, arg2, arg3);
        else if (strcmp(cmd, "DELETEGROUP") == 0 && n >= 1) delete_group(arg1);
        else if (strcmp(cmd, "COUNT") == 0 && n >= 1) count_group(arg1);
        else if (strcmp(cmd, "REPORT") == 0) report_groups();
    }
    free_all();
    return 0;
}