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
    Description *descriptions;
    struct Group *next;
} Group;

Group *head = NULL;

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
        if (!g) return;
        g->name = strdup(group);
        g->descriptions = NULL;
        g->next = head;
        head = g;
    }
    Description *d = malloc(sizeof(Description));
    if (!d) return;
    d->text = strdup(desc);
    d->next = g->descriptions;
    g->descriptions = d;
}

void merge_groups(const char *groupA, const char *groupB, const char *newGroup) {
    Group *a = find_group(groupA);
    Group *b = find_group(groupB);
    if (!a || !b) return;
    if (find_group(newGroup)) return;
    Group *ng = malloc(sizeof(Group));
    if (!ng) return;
    ng->name = strdup(newGroup);
    ng->descriptions = NULL;
    Description *d = a->descriptions;
    while (d) {
        Description *nd = malloc(sizeof(Description));
        if (nd) {
            nd->text = strdup(d->text);
            nd->next = ng->descriptions;
            ng->descriptions = nd;
        }
        d = d->next;
    }
    d = b->descriptions;
    while (d) {
        Description *nd = malloc(sizeof(Description));
        if (nd) {
            nd->text = strdup(d->text);
            nd->next = ng->descriptions;
            ng->descriptions = nd;
        }
        d = d->next;
    }
    ng->next = head;
    head = ng;
}

void delete_group(const char *name) {
    Group *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            Description *d = curr->descriptions;
            while (d) {
                Description *tmp = d;
                d = d->next;
                free(tmp->text);
                free(tmp);
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
    int cnt = 0;
    Description *d = g->descriptions;
    while (d) { cnt++; d = d->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = head;
    while (g) {
        printf("%s", g->name);
        Description *d = g->descriptions;
        while (d) {
            printf(" %s", d->text);
            d = d->next;
        }
        printf("\n");
        g = g->next;
    }
}

void free_all(void) {
    Group *g = head;
    while (g) {
        Group *tmp = g;
        g = g->next;
        Description *d = tmp->descriptions;
        while (d) {
            Description *tmpd = d;
            d = d->next;
            free(tmpd->text);
            free(tmpd);
        }
        free(tmp->name);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char group[256];
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", group) == 1) {
                char *desc = p;
                while (*desc && *desc != ' ') desc++;
                while (*desc && *desc == ' ') desc++;
                if (*desc) {
                    add_event(group, desc);
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char a[256], b[256], ng[256];
            if (sscanf(line + 6, "%255s %255s %255s", a, b, ng) == 3) {
                merge_groups(a, b, ng);
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *p = line + 12;
            while (*p == ' ') p++;
            delete_group(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            count_group(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}