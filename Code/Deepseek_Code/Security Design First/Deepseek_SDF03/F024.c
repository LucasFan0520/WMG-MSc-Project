// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TITLE 64
#define MAX_AUTHOR 64

typedef struct Author {
    char name[MAX_AUTHOR];
    struct Author *next;
} Author;

typedef struct Book {
    char title[MAX_TITLE];
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

void free_authors(Author *a) {
    while (a) {
        Author *tmp = a;
        a = a->next;
        free(tmp);
    }
}

void free_all() {
    Book *b = head;
    while (b) {
        Book *tmp = b;
        free_authors(b->authors);
        b = b->next;
        free(tmp);
    }
    head = NULL;
}

void handle_book(char *line) {
    char title[MAX_TITLE];
    int year;
    if (sscanf(line, "BOOK %63s %d", title, &year) != 2) return;
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    strcpy(b->title, title);
    b->year = year;
    b->authors = NULL;
    b->next = head;
    head = b;
}

void handle_author(char *line) {
    char title[MAX_TITLE], name[MAX_AUTHOR];
    if (sscanf(line, "AUTHOR %63s %63s", title, name) != 2) return;
    Book *b = find_book(title);
    if (!b) return;
    if (find_author(b, name)) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
    strcpy(a->name, name);
    a->next = b->authors;
    b->authors = a;
}

void handle_removeauthor(char *line) {
    char title[MAX_TITLE], name[MAX_AUTHOR];
    if (sscanf(line, "REMOVEAUTHOR %63s %63s", title, name) != 2) return;
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors, *prev = NULL;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else b->authors = a->next;
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void handle_delete(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "DELETE %63s", title) != 1) return;
    Book *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free_authors(b->authors);
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void handle_find(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "FIND %63s", title) != 1) return;
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors;
    int first = 1;
    while (a) {
        if (!first) putchar(' ');
        printf("%s", a->name);
        first = 0;
        a = a->next;
    }
    putchar('\n');
}

void handle_list() {
    Book *b = head;
    while (b) {
        printf("%s\n", b->title);
        b = b->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "BOOK ", 5) == 0) handle_book(line);
        else if (strncmp(line, "AUTHOR ", 7) == 0) handle_author(line);
        else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) handle_removeauthor(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}