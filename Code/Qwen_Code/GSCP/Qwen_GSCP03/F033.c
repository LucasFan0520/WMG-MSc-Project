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

void free_list() {
    while (head) {
        Emp *e = head; head = head->next;
        free(e->id); free(e->name); free(e->dept); free(e->title); free(e);
    }
}

Emp *find_emp(const char *id) {
    for (Emp *e = head; e; e = e->next)
        if (strcmp(e->id, id) == 0) return e;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], id[256], name[256], dept[256], title[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) != 4) continue;
            if (find_emp(id)) continue;
            Emp *e = malloc(sizeof(Emp));
            e->id = strdup(id); e->name = strdup(name); e->dept = strdup(dept); e->title = strdup(title);
            e->next = head; head = e;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) != 4) continue;
            Emp *e = find_emp(id);
            if (e) {
                free(e->name); free(e->dept); free(e->title);
                e->name = strdup(name); e->dept = strdup(dept); e->title = strdup(title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) != 1) continue;
            Emp *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->id, id) == 0) {
                    Emp *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->id); free(tmp->name); free(tmp->dept); free(tmp->title); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", id) != 1) continue;
            Emp *e = find_emp(id);
            if (e) printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Emp *e = head; e; e = e->next) {
                printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
            }
        }
    }
    free_list();
    return 0;
}