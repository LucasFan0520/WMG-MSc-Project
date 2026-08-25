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

Book *books = NULL;

Book *find_book(const char *title) {
    Book *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors;
    while (a && strcmp(a->name, name) != 0) a = a->next;
    if (a) return;
    Author *na = malloc(sizeof(Author));
    na->name = strdup(name);
    na->next = b->authors;
    b->authors = na;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors, *prev = NULL;
    while (a && strcmp(a->name, name) != 0) {
        prev = a;
        a = a->next;
    }
    if (!a) return;
    if (prev) prev->next = a->next;
    else b->authors = a->next;
    free(a->name);
    free(a);
}

void delete_book(const char *title) {
    Book *b = books, *prev = NULL;
    while (b && strcmp(b->title, title) != 0) {
        prev = b;
        b = b->next;
    }
    if (!b) return;
    if (prev) prev->next = b->next;
    else books = b->next;
    free(b->title);
    Author *a = b->authors;
    while (a) {
        Author *next = a->next;
        free(a->name);
        free(a);
        a = next;
    }
    free(b);
}

void find_book_authors(const char *title) {
    Book *b = find_book(title);
    if (!b) { printf("NOT_FOUND\n"); return; }
    printf("%s %d", b->title, b->year);
    Author *a = b->authors;
    while (a) {
        printf(" %s", a->name);
        a = a->next;
    }
    putchar('\n');
}

void list_books(void) {
    Book *b = books;
    while (b) {
        printf("%s %d", b->title, b->year);
        Author *a = b->authors;
        while (a) {
            printf(" %s", a->name);
            a = a->next;
        }
        putchar('\n');
        b = b->next;
    }
}

void free_all(void) {
    Book *b = books;
    while (b) {
        Book *next = b->next;
        free(b->title);
        Author *a = b->authors;
        while (a) {
            Author *nexta = a->next;
            free(a->name);
            free(a);
            a = nexta;
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
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year;
            if (sscanf(line + 5, "%255s %d", title, &year) == 2)
                add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(line + 7, "%255s %255s", title, name) == 2)
                add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(line + 13, "%255s %255s", title, name) == 2)
                remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_book(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_book_authors(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_all();
    free(line);
    return 0;
}