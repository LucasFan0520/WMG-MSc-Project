// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

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

Book* find_book(const char *title) {
    for (Book *b = books; b; b = b->next)
        if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year;
            if (sscanf(rest, "%255s %d", title, &year) != 2) continue;
            if (find_book(title)) continue;
            Book *b = malloc(sizeof(Book));
            if (!b) continue;
            b->title = strdup(title);
            if (!b->title) { free(b); continue; }
            b->year = year;
            b->authors = NULL;
            b->next = books;
            books = b;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(rest, "%255s %255s", title, name) != 2) continue;
            Book *b = find_book(title);
            if (b) {
                Author *a = malloc(sizeof(Author));
                if (!a) continue;
                a->name = strdup(name);
                if (!a->name) { free(a); continue; }
                a->next = b->authors;
                b->authors = a;
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(rest, "%255s %255s", title, name) != 2) continue;
            Book *b = find_book(title);
            if (b) {
                Author *prev = NULL;
                for (Author *a = b->authors; a; prev = a, a = a->next) {
                    if (strcmp(a->name, name) == 0) {
                        if (prev) prev->next = a->next;
                        else b->authors = a->next;
                        free(a->name);
                        free(a);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Book *prev = NULL;
            for (Book *b = books; b; prev = b, b = b->next) {
                if (strcmp(b->title, title) == 0) {
                    if (prev) prev->next = b->next;
                    else books = b->next;
                    while (b->authors) {
                        Author *a = b->authors;
                        b->authors = a->next;
                        free(a->name);
                        free(a);
                    }
                    free(b->title);
                    free(b);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Book *b = find_book(title);
            if (b) {
                for (Author *a = b->authors; a; a = a->next) printf("%s\n", a->name);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Book *b = books; b; b = b->next) printf("%s %d\n", b->title, b->year);
        }
    }
    while (books) {
        Book *b = books;
        books = books->next;
        while (b->authors) {
            Author *a = b->authors;
            b->authors = a->next;
            free(a->name);
            free(a);
        }
        free(b->title);
        free(b);
    }
    return 0;
}