// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Author {
    char *name;
    struct Author *next;
} Author;

typedef struct Book {
    char *title;
    int year;
    Author *authors;
    struct Book *next;
} Book;

Book *books = NULL;

Book *find_book(const char *title) {
    for (Book *b = books; b; b = b->next) {
        if (strcmp(b->title, title) == 0) return b;
    }
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author **ap = &b->authors;
    while (*ap) {
        if (strcmp((*ap)->name, name) == 0) {
            Author *tmp = *ap;
            *ap = tmp->next;
            free(tmp->name);
            free(tmp);
            return;
        }
        ap = &(*ap)->next;
    }
}

void delete_book(const char *title) {
    Book **bp = &books;
    while (*bp) {
        if (strcmp((*bp)->title, title) == 0) {
            Book *tmp = *bp;
            *bp = tmp->next;
            Author *a = tmp->authors;
            while (a) {
                Author *next_a = a->next;
                free(a->name);
                free(a);
                a = next_a;
            }
            free(tmp->title);
            free(tmp);
            return;
        }
        bp = &(*bp)->next;
    }
}

void find_authors(const char *title) {
    Book *b = find_book(title);
    if (!b) return;
    for (Author *a = b->authors; a; a = a->next) {
        printf("%s\n", a->name);
    }
}

void list_books() {
    for (Book *b = books; b; b = b->next) {
        printf("%s %d\n", b->title, b->year);
    }
}

void cleanup() {
    while (books) {
        Book *tmp = books->next;
        Author *a = books->authors;
        while (a) {
            Author *next_a = a->next;
            free(a->name);
            free(a);
            a = next_a;
        }
        free(books->title);
        free(books);
        books = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        int year;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            if (sscanf(line, "%*s %255s %d", a1, &year) == 2) add_book(a1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) add_author(a1, a2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) remove_author(a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_book(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_authors(a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    cleanup();
    return 0;
}