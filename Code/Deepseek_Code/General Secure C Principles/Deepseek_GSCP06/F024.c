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
    while (a) {
        if (strcmp(a->name, name) == 0) return;
        a = a->next;
    }
    Author *na = malloc(sizeof(Author));
    na->name = strdup(name);
    na->next = b->authors;
    b->authors = na;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *cur = b->authors, *prev = NULL;
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
    Book *b = find_book(title);
    if (!b) return;
    Book *cur = books, *prev = NULL;
    while (cur) {
        if (cur == b) {
            if (prev) prev->next = cur->next;
            else books = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(b->title);
    Author *a = b->authors;
    while (a) {
        Author *tmp = a;
        a = a->next;
        free(tmp->name);
        free(tmp);
    }
    free(b);
}

void find_book_cmd(const char *title) {
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

void cleanup_books(void) {
    Book *b = books;
    while (b) {
        Book *tmp = b;
        b = b->next;
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
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        int year;
        int n = sscanf(line, "%31s %255s %d %1023s", cmd, arg1, &year, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "BOOK") == 0 && n >= 3) {
            add_book(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0 && n >= 3) {
            add_author(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n >= 3) {
            remove_author(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_book(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_book_cmd(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    cleanup_books();
    return 0;
}