// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Emp {
    char *id, *name, *dept, *title;
    struct Emp *next;
} Emp;

Emp *head = NULL;

int main() {
    char cmd[20], id[256], name[256], dept[256], title[256];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            Emp *e = malloc(sizeof(Emp));
            e->id = strdup(id); e->name = strdup(name); e->dept = strdup(dept); e->title = strdup(title); e->next = head; head = e;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            for (Emp *e = head; e; e = e->next) {
                if (strcmp(e->id, id) == 0) {
                    free(e->name); free(e->dept); free(e->title);
                    e->name = strdup(name); e->dept = strdup(dept); e->title = strdup(title);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            Emp *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->id, id) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->id); free(curr->name); free(curr->dept); free(curr->title); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", id);
            for (Emp *e = head; e; e = e->next) if (strcmp(e->id, id) == 0) { printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title); break; }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Emp *e = head; e; e = e->next) printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
        }
    }
    return 0;
}