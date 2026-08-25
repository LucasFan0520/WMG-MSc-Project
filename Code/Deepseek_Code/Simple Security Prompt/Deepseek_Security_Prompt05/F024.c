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

Book *find_book(const char *title) {
    Book *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

Author *find_author(Book *b, const char *name) {
    Author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void book_create(const char *title, int year) {
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
    if (find_author(b, name)) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void author_remove(const char *title, const char *name) {
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

void book_delete(const char *title) {
    Book *b = books, *prev = NULL;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            free(b->title);
            while (b->authors) {
                Author *tmp = b->authors;
                b->authors = b->authors->next;
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
    if (!b) {
        printf("NOT_FOUND\n");
        return;
    }
    Author *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void list_books(void) {
    Book *b = books;
    while (b) {
        printf("%s\n", b->title);
        b = b->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256];
        int n = sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        if (strcmp(cmd, "BOOK") == 0 && n >= 2) {
            int year = atoi(arg2);
            book_create(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0 && n >= 3) {
            author_add(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n >= 3) {
            author_remove(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            book_delete(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_book_authors(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    while (books) {
        Book *tmp = books;
        books = books->next;
        free(tmp->title);
        while (tmp->authors) {
            Author *a = tmp->authors;
            tmp->authors = tmp->authors->next;
            free(a->name);
            free(a);
        }
        free(tmp);
    }
    return 0;
}