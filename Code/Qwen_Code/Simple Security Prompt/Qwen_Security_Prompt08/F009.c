// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct E {
    char *desc;
    struct E *next;
} E;

typedef struct G {
    char *name;
    E *events;
    struct G *next;
} G;

G *head = NULL;

G* find_g(char *name) {
    for (G *p = head; p; p = p->next) {
        if (!strcmp(p->name, name)) {
            return p;
        }
    }
    return NULL;
}

int main() {
    char cmd[20];
    char g1[100];
    char g2[100];
    char g3[100];
    char desc[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "EVENT")) {
            scanf("%s", g1);
            fgets(desc, 1000, stdin);
            desc[strcspn(desc, "\n")] = 0;
            if (desc[0] == ' ') {
                memmove(desc, desc + 1, strlen(desc));
            }
            G *g = find_g(g1);
            if (!g) {
                g = malloc(sizeof(G));
                g->name = strdup(g1);
                g->events = NULL;
                g->next = head;
                head = g;
            }
            E *ne = malloc(sizeof(E));
            ne->desc = strdup(desc);
            ne->next = g->events;
            g->events = ne;
        } else if (!strcmp(cmd, "MERGE")) {
            scanf("%s %s %s", g1, g2, g3);
            G *ng = malloc(sizeof(G));
            ng->name = strdup(g3);
            ng->events = NULL;
            ng->next = head;
            head = ng;
            G *ga = find_g(g1);
            G *gb = find_g(g2);
            E *tail = NULL;
            if (ga) {
                for (E *e = ga->events; e; e = e->next) {
                    E *ne = malloc(sizeof(E));
                    ne->desc = strdup(e->desc);
                    ne->next = NULL;
                    if (!ng->events) {
                        ng->events = tail = ne;
                    } else {
                        tail->next = ne;
                        tail = ne;
                    }
                }
            }
            if (gb) {
                for (E *e = gb->events; e; e = e->next) {
                    E *ne = malloc(sizeof(E));
                    ne->desc = strdup(e->desc);
                    ne->next = NULL;
                    if (!ng->events) {
                        ng->events = tail = ne;
                    } else {
                        tail->next = ne;
                        tail = ne;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETEGROUP")) {
            scanf("%s", g1);
            G *p = head;
            G *prev = NULL;
            while (p) {
                if (!strcmp(p->name, g1)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    E *e = p->events;
                    while (e) {
                        E *tmp = e;
                        e = e->next;
                        free(tmp->desc);
                        free(tmp);
                    }
                    free(p->name);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            scanf("%s", g1);
            G *g = find_g(g1);
            int c = 0;
            if (g) {
                for (E *e = g->events; e; e = e->next) {
                    c++;
                }
            }
            printf("%d\n", c);
        } else if (!strcmp(cmd, "REPORT")) {
            for (G *g = head; g; g = g->next) {
                printf("%s:", g->name);
                for (E *e = g->events; e; e = e->next) {
                    printf(" %s", e->desc);
                }
                printf("\n");
            }
        }
    }
    return 0;
}