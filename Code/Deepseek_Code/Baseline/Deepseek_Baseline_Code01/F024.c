// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct author {
    char *name;
    struct author *next;
} Author;

typedef struct book {
    char *title;
    int year;
    Author *authors;
    struct book *next;
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

void add_author_to_book(Book *b, const char *name) {
    Author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return;
        a = a->next;
    }
    Author *new = malloc(sizeof(Author));
    new->name = strdup(name);
    new->next = b->authors;
    b->authors = new;
}

void remove_author_from_book(Book *b, const char *name) {
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

void book_create(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void delete_book(const char *title) {
    Book *b = books, *prev = NULL;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            free(b->title);
            Author *a = b->authors;
            while (a) { Author *tmp = a; a = a->next; free(tmp->name); free(tmp); }
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256];
        int year;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            if (sscanf(line, "%*s %255s %d", arg1, &year) == 2)
                book_create(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2) {
                Book *b = find_book(arg1);
                if (b) add_author_to_book(b, arg2);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2) {
                Book *b = find_book(arg1);
                if (b) remove_author_from_book(b, arg2);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_book(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                find_book_authors(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    return 0;
}