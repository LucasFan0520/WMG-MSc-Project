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
    for (Book *b = head; b; b = b->next) {
        if (strcmp(b->title, title) == 0) return b;
    }
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    b->title = strdup(title);
    if (!b->title) { free(b); return; }
    b->year = year;
    b->authors = NULL;
    b->next = head;
    head = b;
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
    a->name = strdup(name);
    if (!a->name) { free(a); return; }
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *curr = b->authors;
    Author *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else b->authors = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_book(const char *title) {
    Book *curr = head;
    Book *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            Author *a = curr->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(curr->title);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_book_print(const char *title) {
    Book *b = find_book(title);
    if (!b) return;
    printf("%s %d\n", b->title, b->year);
    for (Author *a = b->authors; a; a = a->next) {
        printf("%s\n", a->name);
    }
}

void list_books() {
    for (Book *b = head; b; b = b->next) {
        printf("%s %d\n", b->title, b->year);
    }
}

void free_all() {
    Book *b = head;
    while (b) {
        Book *next = b->next;
        Author *a = b->authors;
        while (a) {
            Author *next_a = a->next;
            free(a->name);
            free(a);
            a = next_a;
        }
        free(b->title);
        free(b);
        b = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        int year = 0;
        
        int n = sscanf(line, "%63s %1023s %1023s", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "BOOK") == 0) {
            if (sscanf(line, "%*s %1023s %d", arg1, &year) == 2) {
                add_book(arg1, year);
            }
        } else if (strcmp(cmd, "AUTHOR") == 0 && n >= 3) {
            add_author(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n >= 3) {
            remove_author(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_book(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_book_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_all();
    return 0;
}