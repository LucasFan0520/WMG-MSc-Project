// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Desc {
    char *text;
    struct Desc *next;
} Desc;

typedef struct Group {
    char *name;
    Desc *descs;
    struct Group *next;
} Group;

Group *head = NULL;

void free_descs(Desc *d) {
    while (d) { Desc *t = d; d = d->next; free(t->text); free(t); }
}

void free_list() {
    while (head) {
        Group *t = head; head = head->next;
        free(t->name); free_descs(t->descs); free(t);
    }
}

Group *find_group(const char *name) {
    for (Group *g = head; g; g = g->next)
        if (strcmp(g->name, name) == 0) return g;
    return NULL;
}

int count_descs(Desc *d) {
    int c = 0;
    for (; d; d = d->next) c++;
    return c;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], g1[256], g2[256], g3[256], desc[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", g1, desc) < 1) continue;
            Group *g = find_group(g1);
            if (!g) {
                g = malloc(sizeof(Group));
                g->name = strdup(g1); g->descs = NULL; g->next = head; head = g;
            }
            Desc *d = malloc(sizeof(Desc));
            d->text = strdup(desc); d->next = g->descs; g->descs = d;
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", g1, g2, g3) != 3) continue;
            Group *ga = find_group(g1), *gb = find_group(g2);
            if (ga && gb) {
                Group *g = malloc(sizeof(Group));
                g->name = strdup(g3); g->descs = NULL; g->next = head; head = g;
                for (Desc *d = ga->descs; d; d = d->next) {
                    Desc *nd = malloc(sizeof(Desc));
                    nd->text = strdup(d->text); nd->next = g->descs; g->descs = nd;
                }
                for (Desc *d = gb->descs; d; d = d->next) {
                    Desc *nd = malloc(sizeof(Desc));
                    nd->text = strdup(d->text); nd->next = g->descs; g->descs = nd;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(line, "%*s %255s", g1) != 1) continue;
            Group *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, g1) == 0) {
                    Group *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free_descs(tmp->descs); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", g1) != 1) continue;
            Group *g = find_group(g1);
            printf("%d\n", g ? count_descs(g->descs) : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Group *g = head; g; g = g->next) {
                printf("%s:", g->name);
                for (Desc *d = g->descs; d; d = d->next) printf(" %s", d->text);
                printf("\n");
            }
        }
    }
    free_list();
    return 0;
}