// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Emp {
    char *id;
    char *name;
    char *dept;
    char *title;
    struct Emp *next;
} Emp;

Emp *head = NULL;

int main() {
    char cmd[20];
    char id[100], name[100], dept[100], title[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD") || !strcmp(cmd, "UPDATE")) {
            scanf("%s %s %s %s", id, name, dept, title);
            Emp *p = head;
            while (p && strcmp(p->id, id)) p = p->next;
            if (p) {
                free(p->name); free(p->dept); free(p->title);
                p->name = strdup(name);
                p->dept = strdup(dept);
                p->title = strdup(title);
            } else {
                Emp *e = malloc(sizeof(Emp));
                e->id = strdup(id);
                e->name = strdup(name);
                e->dept = strdup(dept);
                e->title = strdup(title);
                e->next = head;
                head = e;
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", id);
            Emp *p = head;
            Emp *prev = NULL;
            while (p) {
                if (!strcmp(p->id, id)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->id); free(p->name); free(p->dept); free(p->title);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", id);
            for (Emp *e = head; e; e = e->next) {
                if (!strcmp(e->id, id)) {
                    printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
                    break;
                }
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (Emp *e = head; e; e = e->next) {
                printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
            }
        }
    }
    return 0;
}