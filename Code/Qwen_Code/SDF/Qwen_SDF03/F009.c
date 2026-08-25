// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Group {
    char *name;
    char **descs;
    size_t count, cap;
    struct Group *next;
} Group;

Group *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Group *find_group(const char *name) {
    for (Group *g = head; g; g = g->next)
        if (strcmp(g->name, name) == 0) return g;
    return NULL;
}

void add_desc(Group *g, const char *desc) {
    if (g->count == g->cap) {
        size_t new_cap = g->cap ? g->cap * 2 : 4;
        char **tmp = realloc(g->descs, new_cap * sizeof(char *));
        if (!tmp) return;
        g->descs = tmp; g->cap = new_cap;
    }
    g->descs[g->count++] = strdup(desc);
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Group *g = find_group(name);
                if (!g) {
                    g = malloc(sizeof(Group));
                    if (g) { g->name = strdup(name); g->descs = NULL; g->count = g->cap = 0; g->next = head; head = g; }
                }
                if (g) add_desc(g, ptr);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *a = get_token(&ptr);
            char *b = get_token(&ptr);
            char *n = get_token(&ptr);
            if (a && b && n) {
                Group *ga = find_group(a), *gb = find_group(b);
                if (ga && gb) {
                    Group *gn = find_group(n);
                    if (!gn) {
                        gn = malloc(sizeof(Group));
                        if (gn) { gn->name = strdup(n); gn->descs = NULL; gn->count = gn->cap = 0; gn->next = head; head = gn; }
                    }
                    if (gn) {
                        for (size_t i = 0; i < ga->count; i++) add_desc(gn, ga->descs[i]);
                        for (size_t i = 0; i < gb->count; i++) add_desc(gn, gb->descs[i]);
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Group *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        for (size_t i = 0; i < curr->count; i++) free(curr->descs[i]);
                        free(curr->descs); free(curr->name); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Group *g = find_group(name);
                printf("%zu\n", g ? g->count : 0);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Group *g = head; g; g = g->next) {
                printf("%s:", g->name);
                for (size_t i = 0; i < g->count; i++) printf(" %s", g->descs[i]);
                printf("\n");
            }
        }
    }
    while (head) {
        Group *next = head->next;
        for (size_t i = 0; i < head->count; i++) free(head->descs[i]);
        free(head->descs); free(head->name); free(head);
        head = next;
    }
    return 0;
}