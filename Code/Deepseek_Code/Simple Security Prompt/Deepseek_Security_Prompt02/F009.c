/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct EventDesc {
    char *text;
    struct EventDesc *next;
} EventDesc;

typedef struct Group {
    char *name;
    EventDesc *descriptions;
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

void free_event_descs(EventDesc *e) {
    while (e) {
        EventDesc *tmp = e;
        e = e->next;
        free(tmp->text);
        free(tmp);
    }
}

void free_group(Group *g) {
    free(g->name);
    free_event_descs(g->descriptions);
    free(g);
}

Group *add_group(const char *name) {
    Group *g = malloc(sizeof(Group));
    g->name = strdup(name);
    g->descriptions = NULL;
    g->next = groups;
    groups = g;
    return g;
}

void event_add(const char *group, const char *desc) {
    Group *g = find_group(group);
    if (!g) g = add_group(group);
    EventDesc *e = malloc(sizeof(EventDesc));
    e->text = strdup(desc);
    e->next = g->descriptions;
    g->descriptions = e;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *ng = add_group(newg);
    EventDesc *e = ga->descriptions;
    while (e) {
        EventDesc *ne = malloc(sizeof(EventDesc));
        ne->text = strdup(e->text);
        ne->next = ng->descriptions;
        ng->descriptions = ne;
        e = e->next;
    }
    e = gb->descriptions;
    while (e) {
        EventDesc *ne = malloc(sizeof(EventDesc));
        ne->text = strdup(e->text);
        ne->next = ng->descriptions;
        ng->descriptions = ne;
        e = e->next;
    }
}

void delete_group(const char *name) {
    Group *prev = NULL, *curr = groups;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else groups = curr->next;
            free_group(curr);
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
    int cnt = 0;
    EventDesc *e = g->descriptions;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        EventDesc *e = g->descriptions;
        while (e) {
            printf("  %s\n", e->text);
            e = e->next;
        }
        g = g->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char group[256], group2[256], newg[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "EVENT") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", group) != 1) continue;
            p += strlen(group);
            while (*p == ' ') p++;
            event_add(group, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s %255s", group, group2, newg) != 3) continue;
            merge_groups(group, group2, newg);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *p = line + 11;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", group) == 1) delete_group(group);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", group) == 1) count_group(group);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }

    while (groups) {
        Group *tmp = groups;
        groups = groups->next;
        free_group(tmp);
    }
    return 0;
}