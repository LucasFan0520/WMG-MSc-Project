/*
 * F024.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct author {
    char *name;
    struct author *next;
} Author;

typedef struct book {
    char *title;
    int year;
    Author *authors;
    struct book *next;
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

void book_create(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void book_add_author(const char *title, const char *name) {
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

void book_remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL, *a = b->authors;
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

void book_delete(const char *title) {
    Book *prev = NULL, *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            free(b->title);
            Author *a = b->authors;
            while (a) {
                Author *tmp = a;
                a = a->next;
                free(tmp->name);
                free(tmp);
            }
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void book_find(const char *title) {
    Book *b = find_book(title);
    if (!b) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %d", b->title, b->year);
    Author *a = b->authors;
    while (a) {
        printf(" %s", a->name);
        a = a->next;
    }
    printf("\n");
}

void book_list(void) {
    Book *b = books;
    while (b) {
        printf("%s\n", b->title);
        b = b->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256];
        if (sscanf(line, "%31s %255s %255s", cmd, arg1, arg2) < 1) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            int year;
            char *rest = line + strlen(cmd) + strlen(arg1) + 1;
            while (*rest == ' ') rest++;
            if (sscanf(rest, "%d", &year) == 1) {
                book_create(arg1, year);
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            book_add_author(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            book_remove_author(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            book_delete(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            book_find(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            book_list();
        }
    }
    return 0;
}