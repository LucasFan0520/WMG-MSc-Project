// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Auth { char *name; struct Auth *next; } Auth;
typedef struct Book { char *title; int year; Auth *auths; struct Book *next; } Book;
Book *books = NULL;

Book *find_book(char *t) { for (Book *b = books; b; b = b->next) if (!strcmp(b->title, t)) return b; return NULL; }

void cmd_book(char *t, int y) { if (find_book(t)) return; Book *b = malloc(sizeof(Book)); b->title = strdup(t); b->year = y; b->auths = NULL; b->next = books; books = b; }
void cmd_author(char *t, char *n) { Book *b = find_book(t); if (!b) return; Auth *a = malloc(sizeof(Auth)); a->name = strdup(n); a->next = b->auths; b->auths = a; }
void cmd_removeauthor(char *t, char *n) {
    Book *b = find_book(t); if (!b) return;
    Auth *curr = b->auths, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else b->auths = curr->next; free(curr->name); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_delete(char *t) {
    Book *curr = books, *prev = NULL;
    while (curr) { if (!strcmp(curr->title, t)) { if (prev) prev->next = curr->next; else books = curr->next; Auth *a = curr->auths; while (a) { Auth *na = a->next; free(a->name); free(a); a = na; } free(curr->title); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_find(char *t) { Book *b = find_book(t); if (!b) return; for (Auth *a = b->auths; a; a = a->next) printf("%s\n", a->name); }
void cmd_list() { for (Book *b = books; b; b = b->next) printf("%s %d\n", b->title, b->year); }
void free_all() {
    Book *b = books; while (b) { Book *nb = b->next; Auth *a = b->auths; while (a) { Auth *na = a->next; free(a->name); free(a); a = na; } free(b->title); free(b); b = nb; }
}
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256]; int y;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "BOOK") && sscanf(line, "%*s %255s %d", a1, &y) == 2) cmd_book(a1, y);
        else if (!strcmp(cmd, "AUTHOR") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_author(a1, a2);
        else if (!strcmp(cmd, "REMOVEAUTHOR") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_removeauthor(a1, a2);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "FIND") && sscanf(line, "%*s %255s", a1) == 1) cmd_find(a1);
        else if (!strcmp(cmd, "LIST")) cmd_list();
    }
    free_all(); return 0;
}