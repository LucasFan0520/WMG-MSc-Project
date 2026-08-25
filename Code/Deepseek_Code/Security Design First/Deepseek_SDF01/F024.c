// filename: F024.c
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

static Book *books = NULL;

static void free_authors(Author *a) {
    while (a) {
        Author *tmp = a;
        a = a->next;
        free(tmp->name);
        free(tmp);
    }
}

static void cleanup(void) {
    while (books) {
        Book *tmp = books;
        books = books->next;
        free(tmp->title);
        free_authors(tmp->authors);
        free(tmp);
    }
}

static Book *find_book(const char *title) {
    for (Book *b = books; b; b = b->next)
        if (strcmp(b->title, title) == 0)
            return b;
    return NULL;
}

static void book_add(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    b->title = strdup(title);
    if (!b->title) { free(b); return; }
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

static void author_add(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
    a->name = strdup(name);
    if (!a->name) { free(a); return; }
    a->next = b->authors;
    b->authors = a;
}

static void author_remove(const char *title, const char *name) {
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

static void book_delete(const char *title) {
    Book *prev = NULL;
    for (Book *b = books; b; b = b->next) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            free(b->title);
            free_authors(b->authors);
            free(b);
            return;
        }
        prev = b;
    }
}

static void find_print(const char *title) {
    Book *b = find_book(title);
    if (!b) return;
    printf("%s %d", b->title, b->year);
    for (Author *a = b->authors; a; a = a->next)
        printf(" %s", a->name);
    putchar('\n');
}

static void list_books(void) {
    for (Book *b = books; b; b = b->next) {
        printf("%s %d", b->title, b->year);
        for (Author *a = b->authors; a; a = a->next)
            printf(" %s", a->name);
        putchar('\n');
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        int year = atoi(arg2);
        if (strcmp(cmd, "BOOK") == 0) {
            if (*arg1) book_add(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (*arg1 && *arg2) author_add(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (*arg1 && *arg2) author_remove(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) book_delete(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free(line);
    return 0;
}