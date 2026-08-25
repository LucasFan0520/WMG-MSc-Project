// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rem { int pri; char *title; char *note; struct Rem *next; } Rem;
Rem *rems = NULL, *tail = NULL;

void cmd_add(int p, char *t, char *n) { Rem *r = malloc(sizeof(Rem)); r->pri = p; r->title = strdup(t); r->note = strdup(n); r->next = NULL; if (tail) tail->next = r; else rems = r; tail = r; }
void cmd_bump(char *t, int d) { for (Rem *r = rems; r; r = r->next) if (!strcmp(r->title, t)) { r->pri += d; return; } }
void cmd_remove(char *t) {
    Rem *curr = rems, *prev = NULL;
    while (curr) { if (!strcmp(curr->title, t)) { if (prev) prev->next = curr->next; else rems = curr->next; if (tail == curr) tail = prev; free(curr->title); free(curr->note); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_top(int n) {
    int count = 0; for (Rem *r = rems; r; r = r->next) count++;
    Rem **arr = malloc(count * sizeof(Rem*)); int i = 0;
    for (Rem *r = rems; r; r = r->next) arr[i++] = r;
    for (i = 0; i < count - 1; i++) for (int j = 0; j < count - i - 1; j++) if (arr[j]->pri < arr[j+1]->pri) { Rem *tmp = arr[j]; arr[j] = arr[j+1]; arr[j+1] = tmp; }
    for (i = 0; i < n && i < count; i++) printf("%d %s %s\n", arr[i]->pri, arr[i]->title, arr[i]->note);
    free(arr);
}
void cmd_list() { for (Rem *r = rems; r; r = r->next) printf("%d %s %s\n", r->pri, r->title, r->note); }
void free_all() { Rem *r = rems; while (r) { Rem *nr = r->next; free(r->title); free(r->note); free(r); r = nr; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0}; int p, n;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %d %255s %4095[^\n]", &p, a1, a2) >= 2) cmd_add(p, a1, a2);
        else if (!strcmp(cmd, "BUMP") && sscanf(line, "%*s %255s %d", a1, &n) == 2) cmd_bump(a1, n);
        else if (!strcmp(cmd, "REMOVE") && sscanf(line, "%*s %255s", a1) == 1) cmd_remove(a1);
        else if (!strcmp(cmd, "TOP") && sscanf(line, "%*s %d", &n) == 1) cmd_top(n);
        else if (!strcmp(cmd, "LIST")) cmd_list();
    }
    free_all(); return 0;
}