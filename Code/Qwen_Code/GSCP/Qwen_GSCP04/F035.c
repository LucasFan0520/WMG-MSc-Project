// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job { char *name; char *payload; struct Job *next; } Job;
Job *jobs = NULL;

Job *find(char *n) { for (Job *j = jobs; j; j = j->next) if (!strcmp(j->name, n)) return j; return NULL; }
void cmd_enqueue(char *n, char *p) { if (find(n)) return; Job *j = malloc(sizeof(Job)); j->name = strdup(n); j->payload = strdup(p); j->next = jobs; jobs = j; }
void cmd_clone(char *o, char *n) { Job *j = find(o); if (!j || find(n)) return; Job *nj = malloc(sizeof(Job)); nj->name = strdup(n); nj->payload = strdup(j->payload); nj->next = jobs; jobs = nj; }
void cmd_cancel(char *n) {
    Job *curr = jobs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else jobs = curr->next; free(curr->name); free(curr->payload); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_run(char *n) {
    Job *curr = jobs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else jobs = curr->next; printf("%s\n", curr->payload); free(curr->name); free(curr->payload); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_list() { for (Job *j = jobs; j; j = j->next) printf("%s %s\n", j->name, j->payload); }
void free_all() { Job *j = jobs; while (j) { Job *nj = j->next; free(j->name); free(j->payload); free(j); j = nj; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ENQUEUE") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) >= 1) cmd_enqueue(a1, a2);
        else if (!strcmp(cmd, "CLONE") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_clone(a1, a2);
        else if (!strcmp(cmd, "CANCEL") && sscanf(line, "%*s %255s", a1) == 1) cmd_cancel(a1);
        else if (!strcmp(cmd, "RUN") && sscanf(line, "%*s %255s", a1) == 1) cmd_run(a1);
        else if (!strcmp(cmd, "LIST")) cmd_list();
    }
    free_all(); return 0;
}