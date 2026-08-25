// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note { char *name; char *note; struct Note *next; } Note;
Note *notes = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r"); if (!f) return;
    Note *temp = NULL; char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256], note[4096] = {0};
        if (sscanf(line, "%255s %4095[^\n]", name, note) < 1) {
            while (temp) { Note *n = temp->next; free(temp->name); free(temp->note); free(temp); temp = n; }
            fclose(f); return;
        }
        Note *n = malloc(sizeof(Note)); n->name = strdup(name); n->note = strdup(note); n->next = temp; temp = n;
    }
    fclose(f);
    while (notes) { Note *n = notes->next; free(notes->name); free(notes->note); free(notes); notes = n; }
    notes = temp;
}
void cmd_add(char *n, char *t) { Note *note = malloc(sizeof(Note)); note->name = strdup(n); note->note = strdup(t); note->next = notes; notes = note; }
void cmd_delete(char *n) {
    Note *curr = notes, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else notes = curr->next; free(curr->name); free(curr->note); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_report() { for (Note *n = notes; n; n = n->next) printf("%s %s\n", n->name, n->note); }
void free_all() { Note *n = notes; while (n) { Note *nn = n->next; free(n->name); free(n->note); free(n); n = nn; } }
int main(int argc, char *argv[]) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "LOAD") && argc > 1) load_file(argv[1]);
        else if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) >= 1) cmd_add(a1, a2);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "REPORT")) cmd_report();
    }
    free_all(); return 0;
}