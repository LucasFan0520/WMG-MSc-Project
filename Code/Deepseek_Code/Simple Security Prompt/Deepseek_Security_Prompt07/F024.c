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

Author *find_author_in_book(Book *b, const char *name) {
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
    if (find_author_in_book(b, name)) return;
    Author *a = malloc(sizeof(Author));
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL;
    Author *a = b->authors;
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
    Book *prev = NULL;
    Book *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            Author *a = b->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(b->title);
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
    int first = 1;
    while (a) {
        if (!first) putchar(' ');
        for (char *s = a->name; *s; s++) putchar(*s == ' ' ? '_' : *s);
        first = 0;
        a = a->next;
    }
    putchar('\n');
}

void list_books() {
    Book *b = books;
    while (b) {
        printf("%s:%d\n", b->title, b->year);
        b = b->next;
    }
}

void free_all() {
    Book *b = books;
    while (b) {
        Book *nextb = b->next;
        Author *a = b->authors;
        while (a) {
            Author *nexta = a->next;
            free(a->name);
            free(a);
            a = nexta;
        }
        free(b->title);
        free(b);
        b = nextb;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year;
            if (sscanf(rest, "%255s %d", title, &year) == 2) {
                add_book(title, year);
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(rest, "%255s %255s", title, name) == 2) {
                add_author(title, name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(rest, "%255s %255s", title, name) == 2) {
                remove_author(title, name);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_book(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_book_authors(rest);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_all();
    return 0;
}