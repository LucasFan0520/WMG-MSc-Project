/* F024.c */
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
    Author *a = b->authors;
    while (a) { if (strcmp(a->name, name) == 0) return; a = a->next; }
    Author *na = malloc(sizeof(Author));
    na->name = strdup(name);
    na->next = b->authors;
    b->authors = na;
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
    if (!b) { printf("NOT_FOUND\n"); return; }
    Author *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void list_books() {
    Book *b = books;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], title[256], name[256];
        int year;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_books(); continue; }
        if (strcmp(cmd, "BOOK") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &year) == 2)
                add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", title, name) == 2)
                add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", title, name) == 2)
                remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_book_authors(title);
        }
    }
    return 0;
}