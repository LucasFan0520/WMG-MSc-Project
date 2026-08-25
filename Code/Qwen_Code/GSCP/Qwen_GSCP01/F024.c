// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AuthorNode {
    char *name;
    struct AuthorNode *next;
} AuthorNode;

typedef struct Book {
    char *title;
    int year;
    AuthorNode *authors;
    struct Book *next;
} Book;

Book *books = NULL;

Book *find_book(const char *title) {
    Book *curr = books;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) exit(EXIT_FAILURE);
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    AuthorNode *a = malloc(sizeof(AuthorNode));
    if (!a) exit(EXIT_FAILURE);
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    AuthorNode *curr = b->authors;
    AuthorNode *prev = NULL;
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
    Book *curr = books;
    Book *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else books = curr->next;
            AuthorNode *a = curr->authors;
            while (a) {
                AuthorNode *next_a = a->next;
                free(a->name);
                free(a);
                a = next_a;
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
    AuthorNode *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void list_books() {
    Book *curr = books;
    while (curr) {
        printf("%s %d\n", curr->title, curr->year);
        curr = curr->next;
    }
}

void free_all() {
    Book *curr = books;
    while (curr) {
        Book *next_b = curr->next;
        AuthorNode *a = curr->authors;
        while (a) {
            AuthorNode *next_a = a->next;
            free(a->name);
            free(a);
            a = next_a;
        }
        free(curr->title);
        free(curr);
        curr = next_b;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        int year = 0;
        
        int matched = sscanf(line, "%31s %255s %255s %d", cmd, arg1, arg2, &year);
        if (matched < 2) continue;
        
        if (strcmp(cmd, "BOOK") == 0 && matched >= 3) {
            create_book(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0 && matched >= 3) {
            add_author(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && matched >= 3) {
            remove_author(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_book(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_book_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_all();
    return 0;
}