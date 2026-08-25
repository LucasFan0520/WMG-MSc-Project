/* F024.c */
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

Book* find_book(const char *title) {
    Book *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

Author* find_author(Book *b, const char *name) {
    Author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
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
    if (find_author(b, name)) return;
    Author *a = malloc(sizeof(Author));
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors, *prev = NULL;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else b->authors = a->next;
            free(a->name);
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void delete_book(const char *title) {
    Book *b = books, *prev = NULL;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            free(b->title);
            Author *a = b->authors;
            while (a) {
                Author *tmp = a;
                a = a->next;
                free(tmp->name);
                free(tmp);
            }
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void find_book_authors(const char *title) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void list_books() {
    Book *b = books;
    while (b) {
        printf("%s\n", b->title);
        b = b->next;
    }
}

void free_all() {
    while (books) {
        Book *tmp = books;
        books = books->next;
        free(tmp->title);
        Author *a = tmp->authors;
        while (a) {
            Author *atmp = a;
            a = a->next;
            free(atmp->name);
            free(atmp);
        }
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], name[256];
        int year;
        int n = sscanf(line, "%31s %255s %d %255s", cmd, title, &year, name);
        if (strcmp(cmd, "BOOK") == 0 && n >= 3) {
            add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0 && n >= 3) {
            add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n >= 3) {
            remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_books();
        }
    }
    free_all();
    return 0;
}