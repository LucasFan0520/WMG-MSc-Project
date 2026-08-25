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
    for (Group *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) return curr;
    }
    return NULL;
}

void add_event(const char *group, const char *desc) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) return;
        g->name = strdup(group);
        g->events = NULL;
        g->next = head;
        head = g;
    }
    Event *e = malloc(sizeof(Event));
    if (!e) return;
    e->desc = strdup(desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *gA, const char *gB, const char *newG) {
    Group *A = find_group(gA);
    Group *B = find_group(gB);
    if (!A || !B) return;
    
    Group *nG = find_group(newG);
    if (!nG) {
        nG = malloc(sizeof(Group));
        if (!nG) return;
        nG->name = strdup(newG);
        nG->events = NULL;
        nG->next = head;
        head = nG;
    }
    
    for (Event *curr = A->events; curr; curr = curr->next) {
        Event *e = malloc(sizeof(Event));
        if (!e) return;
        e->desc = strdup(curr->desc);
        e->next = nG->events;
        nG->events = e;
    }
    for (Event *curr = B->events; curr; curr = curr->next) {
        Event *e = malloc(sizeof(Event));
        if (!e) return;
        e->desc = strdup(curr->desc);
        e->next = nG->events;
        nG->events = e;
    }
}

void delete_group(const char *name) {
    Group *curr = head;
    Group *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            Group *next = curr->next;
            Event *e = curr->events;
            while (e) {
                Event *en = e->next;
                free(e->desc);
                free(e);
                e = en;
            }
            free(curr->name);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int count = 0;
    if (g) {
        for (Event *e = g->events; e; e = e->next) count++;
    }
    printf("%d\n", count);
}

void report_groups() {
    for (Group *curr = head; curr; curr = curr->next) {
        printf("%s\n", curr->name);
    }
}

void free_all() {
    Group *curr = head;
    while (curr) {
        Group *next = curr->next;
        Event *e = curr->events;
        while (e) {
            Event *en = e->next;
            free(e->desc);
            free(e);
            e = en;
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
        
        char cmd[16];
        char arg1[1024], arg2[1024], arg3[1024];
        
        int n = sscanf(line, "%15s %1023s %1023[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "EVENT") == 0 && n == 3) {
            add_event(arg1, arg2);
        } else if (strcmp(cmd, "MERGE") == 0) {
            sscanf(line, "%*s %1023s %1023s %1023s", arg1, arg2, arg3);
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0 && n >= 2) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}