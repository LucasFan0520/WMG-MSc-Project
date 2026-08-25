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

Book* find_book(char *title) {
    for (Book *b = head; b; b = b->next) {
        if (!strcmp(b->title, title)) return b;
    }
    return NULL;
}

int main() {
    char cmd[20];
    char title[100];
    char name[100];
    int year;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "BOOK")) {
            scanf("%s %d", title, &year);
            Book *b = malloc(sizeof(Book));
            b->title = strdup(title);
            b->year = year;
            b->authors = NULL;
            b->next = head;
            head = b;
        } else if (!strcmp(cmd, "AUTHOR")) {
            scanf("%s %s", title, name);
            Book *b = find_book(title);
            if (b) {
                Auth *a = malloc(sizeof(Auth));
                a->name = strdup(name);
                a->next = b->authors;
                b->authors = a;
            }
        } else if (!strcmp(cmd, "REMOVEAUTHOR")) {
            scanf("%s %s", title, name);
            Book *b = find_book(title);
            if (b) {
                Auth *p = b->authors;
                Auth *prev = NULL;
                while (p) {
                    if (!strcmp(p->name, name)) {
                        if (prev) prev->next = p->next;
                        else b->authors = p->next;
                        free(p->name);
                        free(p);
                        break;
                    }
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", title);
            Book *p = head;
            Book *prev = NULL;
            while (p) {
                if (!strcmp(p->title, title)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Auth *a = p->authors;
                    while (a) {
                        Auth *tmp = a;
                        a = a->next;
                        free(tmp->name);
                        free(tmp);
                    }
                    free(p->title);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", title);
            Book *b = find_book(title);
            if (b) {
                for (Auth *a = b->authors; a; a = a->next) {
                    printf("%s\n", a->name);
                }
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (Book *b = head; b; b = b->next) {
                printf("%s %d\n", b->title, b->year);
            }
        }
    }
    return 0;
}