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

Book *head = NULL;

Book *find_book(char *title) {
    for (Book *b = head; b; b = b->next) {
        if (strcmp(b->title, title) == 0) return b;
    }
    return NULL;
}

void create_book(char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = head;
    head = b;
}

void add_author(char *title, char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(char *title, char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL;
    Author *curr = b->authors;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else b->authors = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_book(char *title) {
    Book *prev = NULL;
    Book *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->title);
            Author *a = curr->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_book_authors(char *title) {
    Book *b = find_book(title);
    if (!b) return;
    for (Author *a = b->authors; a; a = a->next) {
        printf("%s\n", a->name);
    }
}

void list_books() {
    for (Book *b = head; b; b = b->next) {
        printf("%s %d\n", b->title, b->year);
    }
}

int main() {
    char cmd[20], a[100], b[100];
    int year;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BOOK") == 0) {
            scanf("%99s%d", a, &year);
            create_book(a, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            scanf("%99s%99s", a, b);
            add_author(a, b);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            scanf("%99s%99s", a, b);
            remove_author(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_book(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            find_book_authors(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    return 0;
}