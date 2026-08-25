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

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

Group *find_group(const char *name) {
    for (Group *g = head; g; g = g->next) if (strcmp(g->name, name) == 0) return g;
    return NULL;
}

Group *create_group(const char *name) {
    Group *g = malloc(sizeof(Group));
    g->name = strdup(name); g->descs = NULL; g->next = head; head = g;
    return g;
}

void add_desc(Group *g, const char *text) {
    Desc *d = malloc(sizeof(Desc));
    d->text = strdup(text); d->next = g->descs; g->descs = d;
}

int main() {
    char cmd[20], g1[256], g2[256], g3[256], desc[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "EVENT") == 0) {
            scanf("%s", g1); read_rest(desc);
            Group *g = find_group(g1);
            if (!g) g = create_group(g1);
            add_desc(g, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s %s", g1, g2, g3);
            Group *ng = find_group(g3);
            if (!ng) ng = create_group(g3);
            Group *ga = find_group(g1), *gb = find_group(g2);
            if (ga) for (Desc *d = ga->descs; d; d = d->next) add_desc(ng, d->text);
            if (gb) for (Desc *d = gb->descs; d; d = d->next) add_desc(ng, d->text);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            scanf("%s", g1);
            Group *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, g1) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->descs) { Desc *nd = curr->descs->next; free(curr->descs->text); free(curr->descs); curr->descs = nd; }
                    free(curr->name); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", g1);
            Group *g = find_group(g1);
            int cnt = 0;
            if (g) for (Desc *d = g->descs; d; d = d->next) cnt++;
            printf("%d\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Group *g = head; g; g = g->next) {
                printf("%s:", g->name);
                for (Desc *d = g->descs; d; d = d->next) printf(" %s", d->text);
                printf("\n");
            }
        }
    }
    return 0;
}