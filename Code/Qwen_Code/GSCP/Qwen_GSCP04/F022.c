// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task { char *title; char *note; int done; struct Task *next; } Task;
typedef struct Proj { char *name; Task *tasks; struct Proj *next; } Proj;
Proj *projs = NULL;

Proj *find_proj(char *n) { for (Proj *p = projs; p; p = p->next) if (!strcmp(p->name, n)) return p; return NULL; }
Task *find_task(Proj *p, char *t) { for (Task *tk = p->tasks; tk; tk = tk->next) if (!strcmp(tk->title, t)) return tk; return NULL; }

void cmd_project(char *n) { if (find_proj(n)) return; Proj *p = malloc(sizeof(Proj)); p->name = strdup(n); p->tasks = NULL; p->next = projs; projs = p; }
void cmd_task(char *pn, char *t, char *n) { Proj *p = find_proj(pn); if (!p) return; Task *tk = malloc(sizeof(Task)); tk->title = strdup(t); tk->note = strdup(n); tk->done = 0; tk->next = p->tasks; p->tasks = tk; }
void cmd_movetask(char *fp, char *tp, char *t) {
    Proj *p1 = find_proj(fp), *p2 = find_proj(tp); if (!p1 || !p2) return;
    Task *curr = p1->tasks, *prev = NULL;
    while (curr) { if (!strcmp(curr->title, t)) { if (prev) prev->next = curr->next; else p1->tasks = curr->next; curr->next = p2->tasks; p2->tasks = curr; return; } prev = curr; curr = curr->next; }
}
void cmd_donetask(char *pn, char *t) { Proj *p = find_proj(pn); if (!p) return; Task *tk = find_task(p, t); if (tk) tk->done = 1; }
void cmd_deleteproject(char *n) {
    Proj *curr = projs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else projs = curr->next; Task *tk = curr->tasks; while (tk) { Task *nt = tk->next; free(tk->title); free(tk->note); free(tk); tk = nt; } free(curr->name); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_report() {
    for (Proj *p = projs; p; p = p->next) {
        for (Task *tk = p->tasks; tk; tk = tk->next) printf("%s %s %s %s\n", p->name, tk->title, tk->done ? "DONE" : "TODO", tk->note);
    }
}
void free_all() {
    Proj *p = projs; while (p) { Proj *np = p->next; Task *tk = p->tasks; while (tk) { Task *nt = tk->next; free(tk->title); free(tk->note); free(tk); tk = nt; } free(p->name); free(p); p = np; }
}
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256], a3[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "PROJECT") && sscanf(line, "%*s %255s", a1) == 1) cmd_project(a1);
        else if (!strcmp(cmd, "TASK") && sscanf(line, "%*s %255s %255s %4095[^\n]", a1, a2, a3) >= 2) cmd_task(a1, a2, a3);
        else if (!strcmp(cmd, "MOVETASK") && sscanf(line, "%*s %255s %255s %255s", a1, a2, a3) == 3) cmd_movetask(a1, a2, a3);
        else if (!strcmp(cmd, "DONETASK") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_donetask(a1, a2);
        else if (!strcmp(cmd, "DELETEPROJECT") && sscanf(line, "%*s %255s", a1) == 1) cmd_deleteproject(a1);
        else if (!strcmp(cmd, "REPORT")) cmd_report();
    }
    free_all(); return 0;
}