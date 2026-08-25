// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Emp { char *id; char *name; char *dept; char *title; struct Emp *next; } Emp;
Emp *emps = NULL;

Emp *find(char *id) { for (Emp *e = emps; e; e = e->next) if (!strcmp(e->id, id)) return e; return NULL; }
void cmd_add(char *id, char *n, char *d, char *t) {
    if (find(id)) return;
    Emp *e = malloc(sizeof(Emp)); e->id = strdup(id); e->name = strdup(n); e->dept = strdup(d); e->title = strdup(t); e->next = emps; emps = e;
}
void cmd_update(char *id, char *n, char *d, char *t) {
    Emp *e = find(id);
    if (e) { free(e->name); free(e->dept); free(e->title); e->name = strdup(n); e->dept = strdup(d); e->title = strdup(t); }
    else cmd_add(id, n, d, t);
}
void cmd_delete(char *id) {
    Emp *curr = emps, *prev = NULL;
    while (curr) { if (!strcmp(curr->id, id)) { if (prev) prev->next = curr->next; else emps = curr->next; free(curr->id); free(curr->name); free(curr->dept); free(curr->title); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_find(char *id) { Emp *e = find(id); if (e) printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title); }
void cmd_list() { for (Emp *e = emps; e; e = e->next) printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title); }
void free_all() { Emp *e = emps; while (e) { Emp *ne = e->next; free(e->id); free(e->name); free(e->dept); free(e->title); free(e); e = ne; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256], a3[256], a4[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %255s %255s %255s %255s", a1, a2, a3, a4) == 4) cmd_add(a1, a2, a3, a4);
        else if (!strcmp(cmd, "UPDATE") && sscanf(line, "%*s %255s %255s %255s %255s", a1, a2, a3, a4) == 4) cmd_update(a1, a2, a3, a4);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "FIND") && sscanf(line, "%*s %255s", a1) == 1) cmd_find(a1);
        else if (!strcmp(cmd, "LIST")) cmd_list();
    }
    free_all(); return 0;
}