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
        if (!g) exit(EXIT_FAILURE);
        g->name = strdup(group_name);
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    Event *e = malloc(sizeof(Event));
    if (!e) exit(EXIT_FAILURE);
    e->desc = strdup(desc);
    e->next = NULL;
    if (!g->events) {
        g->events = e;
    } else {
        Event *curr = g->events;
        while (curr->next) curr = curr->next;
        curr->next = e;
    }
}

void merge_groups(const char *gA, const char *gB, const char *new_g) {
    Group *a = find_group(gA);
    Group *b = find_group(gB);
    if (!a || !b) return;
    
    Event *curr = a->events;
    while (curr) {
        add_event(new_g, curr->desc);
        curr = curr->next;
    }
    curr = b->events;
    while (curr) {
        add_event(new_g, curr->desc);
        curr = curr->next;
    }
}

void delete_group(const char *name) {
    Group *curr = groups;
    Group *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else groups = curr->next;
            Event *e = curr->events;
            while (e) {
                Event *next_e = e->next;
                free(e->desc);
                free(e);
                e = next_e;
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
    if (!g) {
        printf("0\n");
        return;
    }
    int count = 0;
    Event *e = g->events;
    while (e) {
        count++;
        e = e->next;
    }
    printf("%d\n", count);
}

void report_groups() {
    Group *curr = groups;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
    }
}

void free_all() {
    Group *curr = groups;
    while (curr) {
        Group *next_g = curr->next;
        Event *e = curr->events;
        while (e) {
            Event *next_e = e->next;
            free(e->desc);
            free(e);
            e = next_e;
        }
        free(curr->name);
        free(curr);
        curr = next_g;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char desc[3500] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, desc);
        
        if (strcmp(cmd, "EVENT") == 0 && matched >= 3) {
            char *desc_start = strchr(line, ' ');
            if (desc_start) {
                desc_start = strchr(desc_start + 1, ' ');
                if (desc_start) add_event(arg1, desc_start + 1);
            }
        } else if (strcmp(cmd, "MERGE") == 0 && matched >= 3) {
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}