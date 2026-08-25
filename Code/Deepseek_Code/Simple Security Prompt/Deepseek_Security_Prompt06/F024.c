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
    Book *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void add_author(Book *b, const char *name) {
    Author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return;
        a = a->next;
    }
    Author *new = malloc(sizeof(Author));
    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);
    new->next = b->authors;
    b->authors = new;
}

void remove_author(Book *b, const char *name) {
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

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = malloc(strlen(title) + 1);
    strcpy(b->title, title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void add_author_to_book(const char *title, const char *name) {
    Book *b = find_book(title);
    if (b) add_author(b, name);
}

void remove_author_from_book(const char *title, const char *name) {
    Book *b = find_book(title);
    if (b) remove_author(b, name);
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
                Author *tmp = a->next;
                free(a->name);
                free(a);
                a = tmp;
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

void list_books(void) {
    Book *b = books;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

void free_all(void) {
    Book *b = books;
    while (b) {
        Book *tmp = b->next;
        free(b->title);
        Author *a = b->authors;
        while (a) {
            Author *ta = a->next;
            free(a->name);
            free(a);
            a = ta;
        }
        free(b);
        b = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], name[256];
        int year;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "BOOK") == 0) {
            if (sscanf(rest, "%255s %d", title, &year) == 2) add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (sscanf(rest, "%255s %255s", title, name) == 2) add_author_to_book(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (sscanf(rest, "%255s %255s", title, name) == 2) remove_author_from_book(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", title) == 1) delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", title) == 1) find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_all();
    return 0;
}