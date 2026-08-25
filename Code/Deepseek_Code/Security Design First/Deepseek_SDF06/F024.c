// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Book {
    char *title;
    int year;
    char **authors;
    size_t auth_count;
    size_t auth_cap;
} Book;

static Book *books = NULL;
static size_t book_count = 0;
static size_t book_cap = 0;

static ssize_t find_book(const char *title) {
    for (size_t i = 0; i < book_count; i++)
        if (strcmp(books[i].title, title) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_author(Book *b, const char *name) {
    if (b->auth_count == b->auth_cap) {
        b->auth_cap = b->auth_cap ? b->auth_cap * 2 : 4;
        char **new = realloc(b->authors, b->auth_cap * sizeof(char *));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        b->authors = new;
    }
    b->authors[b->auth_count] = strdup(name);
    if (!b->authors[b->auth_count]) { fprintf(stderr, "Memory error\n"); exit(1); }
    b->auth_count++;
}

static void remove_author(Book *b, const char *name) {
    size_t write = 0;
    for (size_t i = 0; i < b->auth_count; i++) {
        if (strcmp(b->authors[i], name) == 0) {
            free(b->authors[i]);
        } else {
            if (write != i) b->authors[write] = b->authors[i];
            write++;
        }
    }
    b->auth_count = write;
}

static void add_book(const char *title, int year) {
    if (find_book(title) != -1) return;
    if (book_count == book_cap) {
        book_cap = book_cap ? book_cap * 2 : 4;
        Book *new = realloc(books, book_cap * sizeof(Book));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        books = new;
    }
    books[book_count].title = strdup(title);
    if (!books[book_count].title) { fprintf(stderr, "Memory error\n"); exit(1); }
    books[book_count].year = year;
    books[book_count].authors = NULL;
    books[book_count].auth_count = 0;
    books[book_count].auth_cap = 0;
    book_count++;
}

static void delete_book(const char *title) {
    ssize_t idx = find_book(title);
    if (idx == -1) return;
    free(books[idx].title);
    for (size_t i = 0; i < books[idx].auth_count; i++)
        free(books[idx].authors[i]);
    free(books[idx].authors);
    for (size_t i = idx; i < book_count - 1; i++)
        books[i] = books[i + 1];
    book_count--;
}

static void find_book_authors(const char *title) {
    ssize_t idx = find_book(title);
    if (idx == -1) return;
    for (size_t i = 0; i < books[idx].auth_count; i++)
        printf("%s\n", books[idx].authors[i]);
}

static void list_books(void) {
    for (size_t i = 0; i < book_count; i++)
        printf("%s %d\n", books[i].title, books[i].year);
}

static void free_all(void) {
    for (size_t i = 0; i < book_count; i++) {
        free(books[i].title);
        for (size_t j = 0; j < books[i].auth_count; j++)
            free(books[i].authors[j]);
        free(books[i].authors);
    }
    free(books);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], title[256], name[256];
    int year;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "BOOK") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &year) == 2)
                add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", title, name) == 2) {
                ssize_t idx = find_book(title);
                if (idx != -1)
                    add_author(&books[idx], name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", title, name) == 2) {
                ssize_t idx = find_book(title);
                if (idx != -1)
                    remove_author(&books[idx], name);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }

    free(line);
    free_all();
    return 0;
}