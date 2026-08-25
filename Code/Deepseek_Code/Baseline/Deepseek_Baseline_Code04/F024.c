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
    Author *prev = NULL, *cur = b->authors;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else b->authors = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_book(const char *title) {
    Book *prev = NULL, *cur = books;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else books = cur->next;
            free(cur->title);
            Author *a = cur->authors;
            while (a) {
                Author *ta = a;
                a = a->next;
                free(ta->name);
                free(ta);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_authors(const char *title) {
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
        printf("%s\n", b->title);
        b = b->next;
    }
}

void free_books(void) {
    while (books) {
        Book *tmp = books;
        books = books->next;
        free(tmp->title);
        Author *a = tmp->authors;
        while (a) {
            Author *ta = a;
            a = a->next;
            free(ta->name);
            free(ta);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char title[256], name[256];
        int year;
        int n = sscanf(line, "%s %s %d %s", cmd, title, &year, name);
        if (strcmp(cmd, "BOOK") == 0) {
            if (n < 3) continue;
            add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (n < 3) continue;
            add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (n < 3) continue;
            remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            find_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_books();
    return 0;
}