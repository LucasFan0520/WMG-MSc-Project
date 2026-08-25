// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tab { char *name; int rows, cols; char ***cells; struct Tab *next; } Tab;
Tab *tabs = NULL;

void cmd_table(char *n, int r, int c) {
    if (r <= 0 || c <= 0) return;
    Tab *t = malloc(sizeof(Tab)); t->name = strdup(n); t->rows = r; t->cols = c;
    t->cells = malloc(r * sizeof(char**));
    for (int i = 0; i < r; i++) { t->cells[i] = malloc(c * sizeof(char*)); for (int j = 0; j < c; j++) t->cells[i][j] = NULL; }
    t->next = tabs; tabs = t;
}
void cmd_set(char *n, int r, int c, char *v) {
    Tab *t = tabs; while (t) { if (!strcmp(t->name, n)) { if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) { free(t->cells[r][c]); t->cells[r][c] = strdup(v); } return; } t = t->next; }
}
void cmd_get(char *n, int r, int c) {
    Tab *t = tabs; while (t) { if (!strcmp(t->name, n)) { if (r >= 0 && r < t->rows && c >= 0 && c < t->cols && t->cells[r][c]) printf("%s\n", t->cells[r][c]); else printf("EMPTY\n"); return; } t = t->next; }
}
void cmd_delete(char *n) {
    Tab *curr = tabs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else tabs = curr->next; for (int i = 0; i < curr->rows; i++) { for (int j = 0; j < curr->cols; j++) free(curr->cells[i][j]); free(curr->cells[i]); } free(curr->cells); free(curr->name); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_tables() { for (Tab *t = tabs; t; t = t->next) printf("%s\n", t->name); }
void free_all() { Tab *t = tabs; while (t) { Tab *nt = t->next; for (int i = 0; i < t->rows; i++) { for (int j = 0; j < t->cols; j++) free(t->cells[i][j]); free(t->cells[i]); } free(t->cells); free(t->name); free(t); t = nt; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256]; int r, c;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "TABLE") && sscanf(line, "%*s %255s %d %d", a1, &r, &c) == 3) cmd_table(a1, r, c);
        else if (!strcmp(cmd, "SET") && sscanf(line, "%*s %255s %d %d %255s", a1, &r, &c, a2) == 4) cmd_set(a1, r, c, a2);
        else if (!strcmp(cmd, "GET") && sscanf(line, "%*s %255s %d %d", a1, &r, &c) == 3) cmd_get(a1, r, c);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "TABLES")) cmd_tables();
    }
    free_all(); return 0;
}