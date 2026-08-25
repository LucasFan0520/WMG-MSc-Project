// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Auth {
    char *name;
    struct Auth *next;
} Auth;

typedef struct Book {
    char *title;
    int year;
    Auth *authors;
    struct Book *next;
} Book;

Book *head = NULL;

Book *find_book(const char *title) {
    for (Book *b = head; b; b = b->next) if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

int main() {
    char cmd[20], title[256], name[256];
    int year;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BOOK") == 0) {
            scanf("%s %d", title, &year);
            Book *b = malloc(sizeof(Book));
            b->title = strdup(title); b->year = year; b->authors = NULL; b->next = head; head = b;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            scanf("%s %s", title, name);
            Book *b = find_book(title);
            if (b) {
                Auth *a = malloc(sizeof(Auth));
                a->name = strdup(name); a->next = b->authors; b->authors = a;
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            scanf("%s %s", title, name);
            Book *b = find_book(title);
            if (b) {
                Auth *prev = NULL, *curr = b->authors;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else b->authors = curr->next;
                        free(curr->name); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", title);
            Book *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->authors) { Auth *na = curr->authors->next; free(curr->authors->name); free(curr->authors); curr->authors = na; }
                    free(curr->title); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            Book *b = find_book(title);
            if (b) {
                for (Auth *a = b->authors; a; a = a->next) printf("%s\n", a->name);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Book *b = head; b; b = b->next) printf("%s %d\n", b->title, b->year);
        }
    }
    return 0;
}