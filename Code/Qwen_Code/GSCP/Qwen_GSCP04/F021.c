// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ing { char *name; char *note; struct Ing *next; } Ing;
typedef struct Rec { char *name; Ing *ings; struct Rec *next; } Rec;
Rec *recs = NULL;

Rec *find_rec(const char *name) {
    for (Rec *r = recs; r; r = r->next) if (!strcmp(r->name, name)) return r;
    return NULL;
}

void cmd_recipe(char *name) {
    if (find_rec(name)) return;
    Rec *r = malloc(sizeof(Rec));
    r->name = strdup(name); r->ings = NULL; r->next = recs; recs = r;
}

void cmd_ing(char *name, char *ing, char *note) {
    Rec *r = find_rec(name); if (!r) return;
    Ing *i = malloc(sizeof(Ing));
    i->name = strdup(ing); i->note = strdup(note); i->next = r->ings; r->ings = i;
}

void cmd_removeing(char *name, char *ing) {
    Rec *r = find_rec(name); if (!r) return;
    Ing *curr = r->ings, *prev = NULL;
    while (curr) {
        if (!strcmp(curr->name, ing)) {
            if (prev) prev->next = curr->next; else r->ings = curr->next;
            free(curr->name); free(curr->note); free(curr); return;
        }
        prev = curr; curr = curr->next;
    }
}

void cmd_deleterecipe(char *name) {
    Rec *curr = recs, *prev = NULL;
    while (curr) {
        if (!strcmp(curr->name, name)) {
            if (prev) prev->next = curr->next; else recs = curr->next;
            Ing *i = curr->ings;
            while (i) { Ing *n = i->next; free(i->name); free(i->note); free(i); i = n; }
            free(curr->name); free(curr); return;
        }
        prev = curr; curr = curr->next;
    }
}

void cmd_show(char *name) {
    Rec *r = find_rec(name); if (!r) return;
    for (Ing *i = r->ings; i; i = i->next) {
        for (char *c = i->note; *c; c++) if (*c == ' ') putchar('_'); else putchar(*c);
        putchar('\n');
    }
}

void cmd_list() {
    for (Rec *r = recs; r; r = r->next) printf("%s\n", r->name);
}

void free_all() {
    Rec *r = recs;
    while (r) {
        Rec *nr = r->next; Ing *i = r->ings;
        while (i) { Ing *ni = i->next; free(i->name); free(i->note); free(i); i = ni; }
        free(r->name); free(r); r = nr;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256], a3[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "RECIPE") && sscanf(line, "%*s %255s", a1) == 1) cmd_recipe(a1);
        else if (!strcmp(cmd, "ING") && sscanf(line, "%*s %255s %255s %4095[^\n]", a1, a2, a3) >= 2) cmd_ing(a1, a2, a3);
        else if (!strcmp(cmd, "REMOVEING") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_removeing(a1, a2);
        else if (!strcmp(cmd, "DELETERECIPE") && sscanf(line, "%*s %255s", a1) == 1) cmd_deleterecipe(a1);
        else if (!strcmp(cmd, "SHOW") && sscanf(line, "%*s %255s", a1) == 1) cmd_show(a1);
        else if (!strcmp(cmd, "LIST")) cmd_list();
    }
    free_all(); return 0;
}