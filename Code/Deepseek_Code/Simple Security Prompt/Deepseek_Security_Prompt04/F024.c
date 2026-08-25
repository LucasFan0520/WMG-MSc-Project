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

Book *head = NULL;

Book *find_book(char *title) {
    for (Book *b = head; b; b = b->next)
        if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

void add_book(char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = head;
    head = b;
}

void add_author(char *title, char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(char *title, char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL;
    for (Author *a = b->authors; a; a = a->next) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else b->authors = a->next;
            free(a->name);
            free(a);
            return;
        }
        prev = a;
    }
}

void delete_book(char *title) {
    Book *prev = NULL;
    for (Book *b = head; b; b = b->next) {
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
    }
}

void find_authors(char *title) {
    Book *b = find_book(title);
    if (!b) return;
    for (Author *a = b->authors; a; a = a->next)
        printf("%s\n", a->name);
}

void list_books(void) {
    for (Book *b = head; b; b = b->next)
        printf("%s %d\n", b->title, b->year);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256];
        int n = sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "BOOK") == 0 && n == 3) {
            add_book(arg1, atoi(arg2));
        } else if (strcmp(cmd, "AUTHOR") == 0 && n == 3) {
            add_author(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n == 3) {
            remove_author(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_book(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_authors(arg1);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_books();
        }
    }
    free(line);
    free_all();
    return 0;
}