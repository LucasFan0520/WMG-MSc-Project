// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Author { char *name; struct Author *next; } Author;
typedef struct Book { char *title; int year; Author *authors; struct Book *next; } Book;
Book *books = NULL;

Book *find_book(const char *title) {
    for (Book *b = books; b; b = b->next)
        if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

void book_add(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void author_add(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    for (Author *a = b->authors; a; a = a->next)
        if (strcmp(a->name, name) == 0) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL;
    for (Author *a = b->authors; a; prev = a, a = a->next) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next; else b->authors = a->next;
            free(a->name); free(a);
            return;
        }
    }
}

void delete_book(const char *title) {
    Book *prev = NULL;
    for (Book *b = books; b; prev = b, b = b->next) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next; else books = b->next;
            free(b->title);
            while (b->authors) { Author *a = b->authors; b->authors = a->next; free(a->name); free(a); }
            free(b);
            return;
        }
    }
}

void find_book_authors(const char *title) {
    Book *b = find_book(title);
    if (!b) { printf("NOT_FOUND\n"); return; }
    for (Author *a = b->authors; a; a = a->next) printf("%s\n", a->name);
}

void list_books(void) {
    for (Book *b = books; b; b = b->next)
        printf("%s %d\n", b->title, b->year);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "BOOK") == 0) {
            char title[4096]; int year;
            if (sscanf(rest, "%4095s %d", title, &year) == 2)
                book_add(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[4096], name[4096];
            if (sscanf(rest, "%4095s %4095s", title, name) == 2)
                author_add(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[4096], name[4096];
            if (sscanf(rest, "%4095s %4095s", title, name) == 2)
                remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[4096];
            if (sscanf(rest, "%4095s", title) == 1) delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[4096];
            if (sscanf(rest, "%4095s", title) == 1) find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    while (books) { Book *b = books; books = b->next; free(b->title); while (b->authors) { Author *a = b->authors; b->authors = a->next; free(a->name); free(a); } free(b); }
    return 0;
}