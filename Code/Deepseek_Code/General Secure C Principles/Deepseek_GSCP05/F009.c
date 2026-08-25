// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct EventDesc {
    char *text;
    struct EventDesc *next;
} EventDesc;

typedef struct Group {
    char *name;
    EventDesc *events;
    struct Group *next;
} Group;

Group *groups = NULL;

Group *find_group(const char *name) {
    Group *g = groups;
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
        if (!g) return;
        g->name = strdup(group);
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    EventDesc *e = malloc(sizeof(EventDesc));
    if (!e) return;
    e->text = strdup(desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *gnew = malloc(sizeof(Group));
    if (!gnew) return;
    gnew->name = strdup(newg);
    gnew->events = NULL;
    // copy events from a
    EventDesc *e = ga->events;
    while (e) {
        EventDesc *ne = malloc(sizeof(EventDesc));
        if (ne) {
            ne->text = strdup(e->text);
            ne->next = gnew->events;
            gnew->events = ne;
        }
        e = e->next;
    }
    // copy from b
    e = gb->events;
    while (e) {
        EventDesc *ne = malloc(sizeof(EventDesc));
        if (ne) {
            ne->text = strdup(e->text);
            ne->next = gnew->events;
            gnew->events = ne;
        }
        e = e->next;
    }
    gnew->next = groups;
    groups = gnew;
}

void delete_group(const char *name) {
    Group *prev = NULL, *cur = groups;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else groups = cur->next;
            EventDesc *e = cur->events;
            while (e) {
                EventDesc *next = e->next;
                free(e->text);
                free(e);
                e = next;
            }
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void count_events(const char *group) {
    Group *g = find_group(group);
    if (!g) {
        printf("0\n");
        return;
    }
    int cnt = 0;
    EventDesc *e = g->events;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s: ", g->name);
        EventDesc *e = g->events;
        int first = 1;
        while (e) {
            if (!first) putchar(' ');
            char *s = e->text;
            for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
            first = 0;
            e = e->next;
        }
        putchar('\n');
        g = g->next;
    }
}

void free_all(void) {
    Group *g = groups;
    while (g) {
        Group *next = g->next;
        EventDesc *e = g->events;
        while (e) {
            EventDesc *enext = e->next;
            free(e->text);
            free(e);
            e = enext;
        }
        free(g->name);
        free(g);
        g = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *group = p;
            char *space = strchr(group, ' ');
            if (!space) continue;
            *space = '\0';
            char *desc = space + 1;
            while (*desc == ' ') desc++;
            add_event(group, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *ga = p;
            char *space = strchr(ga, ' ');
            if (!space) continue;
            *space = '\0';
            char *gb = space + 1;
            while (*gb == ' ') gb++;
            space = strchr(gb, ' ');
            if (!space) continue;
            *space = '\0';
            char *newg = space + 1;
            while (*newg == ' ') newg++;
            merge_groups(ga, gb, newg);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            char *group = p;
            char *space = strchr(group, ' ');
            if (space) *space = '\0';
            delete_group(group);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *group = p;
            char *space = strchr(group, ' ');
            if (space) *space = '\0';
            count_events(group);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}