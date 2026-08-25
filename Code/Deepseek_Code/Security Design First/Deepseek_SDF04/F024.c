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
    Book *b = head;
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

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = NULL;
    if (!b->title) { free(b); return; }
    if (!head) head = b;
    else {
        Book *last = head;
        while (last->next) last = last->next;
        last->next = b;
    }
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    if (find_author(b, name)) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
    a->name = strdup(name);
    a->next = NULL;
    if (!a->name) { free(a); return; }
    if (!b->authors) b->authors = a;
    else {
        Author *last = b->authors;
        while (last->next) last = last->next;
        last->next = a;
    }
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
    Book *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free(b->title);
            Author *a = b->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
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
    Book *b = head;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

void free_all(void) {
    Book *b = head;
    while (b) {
        Book *next = b->next;
        free(b->title);
        Author *a = b->authors;
        while (a) {
            Author *anext = a->next;
            free(a->name);
            free(a);
            a = anext;
        }
        free(b);
        b = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], title[256], name[256];
        int year;
        if (sscanf(line, "%63s", cmd) != 1) continue;
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
            if (sscanf(line, "%*s %255s", title) == 1)
                delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free(line);
    free_all();
    return 0;
}