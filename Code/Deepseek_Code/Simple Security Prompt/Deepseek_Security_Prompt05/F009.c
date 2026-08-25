/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Description {
    char *text;
    struct Description *next;
} Description;

typedef struct Group {
    char *name;
    Description *descs;
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

void add_description(Group *g, const char *desc) {
    Description *d = malloc(sizeof(Description));
    if (!d) return;
    d->text = strdup(desc);
    d->next = g->descs;
    g->descs = d;
}

void event_group(const char *name, const char *desc) {
    Group *g = find_group(name);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) return;
        g->name = strdup(name);
        g->descs = NULL;
        g->next = groups;
        groups = g;
    }
    add_description(g, desc);
}

void merge_groups(const char *a, const char *b, const char *new) {
    Group *ga = find_group(a), *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(new)) return;
    Group *g = malloc(sizeof(Group));
    if (!g) return;
    g->name = strdup(new);
    g->descs = NULL;
    Description *d = ga->descs;
    while (d) {
        add_description(g, d->text);
        d = d->next;
    }
    d = gb->descs;
    while (d) {
        add_description(g, d->text);
        d = d->next;
    }
    g->next = groups;
    groups = g;
}

void delete_group(const char *name) {
    Group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            while (g->descs) {
                Description *tmp = g->descs;
                g->descs = g->descs->next;
                free(tmp->text);
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
    if (!g) {
        printf("0\n");
        return;
    }
    int cnt = 0;
    Description *d = g->descs;
    while (d) { cnt++; d = d->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        Description *d = g->descs;
        while (d) {
            printf("  %s\n", d->text);
            d = d->next;
        }
        g = g->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *desc = line + strlen(cmd) + strlen(arg1) + 2;
            while (*desc == ' ') desc++;
            event_group(arg1, desc);
        } else if (strcmp(cmd, "MERGE") == 0 && n == 4) {
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    while (groups) {
        Group *tmp = groups;
        groups = groups->next;
        while (tmp->descs) {
            Description *d = tmp->descs;
            tmp->descs = tmp->descs->next;
            free(d->text);
            free(d);
        }
        free(tmp->name);
        free(tmp);
    }
    return 0;
}