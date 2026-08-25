// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; int year; char **authors; int ac, acap; } Book;
Book *books = NULL;
int bc = 0, bcap = 0;

int find_book(const char *title) {
    for (int i = 0; i < bc; i++)
        if (strcmp(books[i].title, title) == 0) return i;
    return -1;
}

int find_author(Book *b, const char *name) {
    for (int i = 0; i < b->ac; i++)
        if (strcmp(b->authors[i], name) == 0) return i;
    return -1;
}

void add_book(const char *title, int year) {
    if (find_book(title) >= 0) return;
    if (bc == bcap) { bcap = bcap ? bcap * 2 : 4; books = realloc(books, sizeof(Book) * bcap); }
    books[bc].title = strdup(title);
    books[bc].year = year;
    books[bc].authors = NULL;
    books[bc].ac = 0;
    books[bc].acap = 0;
    bc++;
}

void add_author(const char *title, const char *name) {
    int idx = find_book(title);
    if (idx < 0) return;
    Book *b = &books[idx];
    if (find_author(b, name) >= 0) return;
    if (b->ac == b->acap) { b->acap = b->acap ? b->acap * 2 : 4; b->authors = realloc(b->authors, sizeof(char *) * b->acap); }
    b->authors[b->ac] = strdup(name);
    b->ac++;
}

void remove_author(const char *title, const char *name) {
    int idx = find_book(title);
    if (idx < 0) return;
    Book *b = &books[idx];
    int ai = find_author(b, name);
    if (ai < 0) return;
    free(b->authors[ai]);
    for (int i = ai; i < b->ac - 1; i++) b->authors[i] = b->authors[i + 1];
    b->ac--;
}

void delete_book(const char *title) {
    int idx = find_book(title);
    if (idx < 0) return;
    for (int i = 0; i < books[idx].ac; i++) free(books[idx].authors[i]);
    free(books[idx].authors);
    free(books[idx].title);
    for (int i = idx; i < bc - 1; i++) books[i] = books[i + 1];
    bc--;
}

void find_book_print(const char *title) {
    int idx = find_book(title);
    if (idx < 0) return;
    for (int i = 0; i < books[idx].ac; i++) printf("%s\n", books[idx].authors[i]);
}

void list_books(void) {
    for (int i = 0; i < bc; i++) printf("%s\n", books[i].title);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            int year;
            if (sscanf(line, "%*s %255s %d", arg1, &year) == 2) add_book(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                add_author(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                remove_author(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_book(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_book_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) list_books();
    }
    for (int i = 0; i < bc; i++) {
        for (int j = 0; j < books[i].ac; j++) free(books[i].authors[j]);
        free(books[i].authors);
        free(books[i].title);
    }
    free(books);
    return 0;
}