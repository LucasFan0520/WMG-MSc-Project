// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

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
    for (Group *g = groups; g; g = g->next) {
        if (strcmp(g->name, name) == 0) return g;
    }
    return NULL;
}

void add_event(const char *group, const char *desc) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group);
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    Event *e = malloc(sizeof(Event));
    e->desc = strdup(desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *gA, const char *gB, const char *newG) {
    Group *A = find_group(gA);
    Group *B = find_group(gB);
    Group *nG = find_group(newG);
    if (!nG) {
        nG = malloc(sizeof(Group));
        nG->name = strdup(newG);
        nG->events = NULL;
        nG->next = groups;
        groups = nG;
    }
    if (A) {
        for (Event *e = A->events; e; e = e->next) {
            Event *ne = malloc(sizeof(Event));
            ne->desc = strdup(e->desc);
            ne->next = nG->events;
            nG->events = ne;
        }
    }
    if (B) {
        for (Event *e = B->events; e; e = e->next) {
            Event *ne = malloc(sizeof(Event));
            ne->desc = strdup(e->desc);
            ne->next = nG->events;
            nG->events = ne;
        }
    }
}

void delete_group(const char *name) {
    Group *prev = NULL, *curr = groups;
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
    int count = 0;
    if (g) {
        for (Event *e = g->events; e; e = e->next) count++;
    }
    printf("%d\n", count);
}

void report_groups() {
    for (Group *g = groups; g; g = g->next) {
        printf("%s\n", g->name);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0}, arg3[256] = {0};
        sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        char *desc = strchr(line, ' ');
        if (desc) {
            desc++;
            while (*desc == ' ') desc++;
            desc = strchr(desc, ' ');
            if (desc) {
                desc++;
                while (*desc == ' ') desc++;
            } else {
                desc = NULL;
            }
        }
        
        if (strcmp(cmd, "EVENT") == 0 && desc) {
            add_event(arg1, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
        free(line);
    }
    
    Group *g = groups;
    while (g) {
        Group *next = g->next;
        Event *e = g->events;
        while (e) {
            Event *next_e = e->next;
            free(e->desc);
            free(e);
            e = next_e;
        }
        free(g->name);
        free(g);
        g = next;
    }
    return 0;
}