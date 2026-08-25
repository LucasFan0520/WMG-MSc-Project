/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Group {
    char *name;
    char **descs;
    size_t dcount, dcap;
    struct Group *next;
} Group;

static Group *groups = NULL;

static Group *find_group(const char *name) {
    Group *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) return g;
        g = g->next;
    }
    return NULL;
}

static void add_desc(Group *g, const char *desc) {
    if (g->dcount == g->dcap) {
        g->dcap = g->dcap ? g->dcap * 2 : 4;
        g->descs = realloc(g->descs, g->dcap * sizeof(char *));
        if (!g->descs) { perror("realloc"); exit(1); }
    }
    g->descs[g->dcount] = strdup(desc);
    g->dcount++;
}

static void add_group(const char *name) {
    Group *g = malloc(sizeof(Group));
    if (!g) { perror("malloc"); exit(1); }
    g->name = strdup(name);
    g->descs = NULL;
    g->dcount = g->dcap = 0;
    g->next = groups;
    groups = g;
}

static void remove_group(const char *name) {
    Group **curr = &groups;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            Group *g = *curr;
            *curr = g->next;
            free(g->name);
            for (size_t i = 0; i < g->dcount; i++) free(g->descs[i]);
            free(g->descs);
            free(g);
            return;
        }
        curr = &(*curr)->next;
    }
}

static void merge_groups(const char *g1, const char *g2, const char *newg) {
    Group *a = find_group(g1);
    Group *b = find_group(g2);
    if (!a || !b) return;
    // remove new group if exists
    remove_group(newg);
    add_group(newg);
    Group *n = find_group(newg);
    if (!n) return;
    for (size_t i = 0; i < a->dcount; i++) add_desc(n, a->descs[i]);
    for (size_t i = 0; i < b->dcount; i++) add_desc(n, b->descs[i]);
}

static void count_group(const char *name) {
    Group *g = find_group(name);
    printf("%zu\n", g ? g->dcount : 0);
}

static void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        for (size_t i = 0; i < g->dcount; i++) {
            printf("  %s\n", g->descs[i]);
        }
        g = g->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = strtok(NULL, " ");
            if (!group) continue;
            char *desc = strtok(NULL, "");
            if (!desc) desc = "";
            Group *g = find_group(group);
            if (!g) { add_group(group); g = find_group(group); }
            if (g) add_desc(g, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *g1 = strtok(NULL, " ");
            char *g2 = strtok(NULL, " ");
            char *ng = strtok(NULL, " ");
            if (g1 && g2 && ng) merge_groups(g1, g2, ng);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = strtok(NULL, " ");
            if (group) remove_group(group);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = strtok(NULL, " ");
            if (group) count_group(group);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    // cleanup not exhaustive but fine
    return 0;
}