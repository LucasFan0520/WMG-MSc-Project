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

Book *find_book(const char *title) {
    for (Book *b = head; b; b = b->next)
        if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = head;
    head = b;
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
    Author *p = b->authors, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else b->authors = p->next;
            free(p->name); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void delete_book(const char *title) {
    Book *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->title, title) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            Author *a = p->authors;
            while (a) {
                Author *na = a->next;
                free(a->name); free(a);
                a = na;
            }
            free(p->title); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void find_book_authors(const char *title) {
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
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000];
        int y;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "BOOK") == 0) {
            sscanf(line, "%*s %s %d", a, &y);
            add_book(a, y);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            add_author(a, b);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            remove_author(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_book(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            find_book_authors(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    while (head) {
        Book *n = head->next;
        Author *a = head->authors;
        while (a) {
            Author *na = a->next;
            free(a->name); free(a);
            a = na;
        }
        free(head->title); free(head);
        head = n;
    }
    return 0;
}