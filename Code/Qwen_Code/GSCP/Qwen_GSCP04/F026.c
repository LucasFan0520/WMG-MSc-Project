// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note { char *cat; char *text; struct Note *next; } Note;
Note *notes = NULL;

void cmd_add(char *c, char *t) { Note *n = malloc(sizeof(Note)); n->cat = strdup(c); n->text = strdup(t); n->next = notes; notes = n; }
void cmd_delete(char *c) {
    Note *curr = notes, *prev = NULL;
    while (curr) { if (!strcmp(curr->cat, c)) { if (prev) prev->next = curr->next; else notes = curr->next; free(curr->cat); free(curr->text); free(curr); if (prev) curr = prev->next; else curr = notes; } else { prev = curr; curr = curr->next; } }
}
void cmd_keep(char *c) {
    Note *curr = notes, *prev = NULL;
    while (curr) { if (strcmp(curr->cat, c)) { if (prev) prev->next = curr->next; else notes = curr->next; free(curr->cat); free(curr->text); free(curr); if (prev) curr = prev->next; else curr = notes; } else { prev = curr; curr = curr->next; } }
}
void cmd_find(char *w) {
    for (Note *n = notes; n; n = n->next) if (strstr(n->text, w)) printf("%s\n", n->text);
}
void cmd_print() { for (Note *n = notes; n; n = n->next) printf("%s %s\n", n->cat, n->text); }
void free_all() { Note *n = notes; while (n) { Note *nn = n->next; free(n->cat); free(n->text); free(n); n = nn; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) >= 1) cmd_add(a1, a2);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "KEEP") && sscanf(line, "%*s %255s", a1) == 1) cmd_keep(a1);
        else if (!strcmp(cmd, "FIND") && sscanf(line, "%*s %255s", a1) == 1) cmd_find(a1);
        else if (!strcmp(cmd, "PRINT")) cmd_print();
    }
    free_all(); return 0;
}