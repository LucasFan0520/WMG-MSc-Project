// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *title; int year; char **authors; size_t ac; size_t acap; } Book;
Book *books = NULL;
size_t bc = 0, bcap = 0;

Book* find_book(const char *title) {
    for (size_t i = 0; i < bc; i++) if (strcmp(books[i].title, title) == 0) return &books[i];
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    if (bc == bcap) {
        size_t nc = bcap ? bcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Book)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Book *tmp = realloc(books, nc * sizeof(Book));
        if (!tmp) { perror("realloc"); exit(1); }
        books = tmp;
        bcap = nc;
    }
    books[bc].title = strdup(title);
    books[bc].year = year;
    books[bc].authors = NULL;
    books[bc].ac = 0;
    books[bc].acap = 0;
    if (!books[bc].title) { perror("strdup"); exit(1); }
    bc++;
}

void add_author(Book *b, const char *name) {
    if (b->ac == b->acap) {
        size_t nc = b->acap ? b->acap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(char*)) { fprintf(stderr, "Overflow\n"); exit(1); }
        char **tmp = realloc(b->authors, nc * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        b->authors = tmp;
        b->acap = nc;
    }
    b->authors[b->ac] = strdup(name);
    if (!b->authors[b->ac]) { perror("strdup"); exit(1); }
    b->ac++;
}

void rem_author(Book *b, const char *name) {
    for (size_t i = 0; i < b->ac; i++) {
        if (strcmp(b->authors[i], name) == 0) {
            free(b->authors[i]);
            b->authors[i] = b->authors[--b->ac];
            return;
        }
    }
}

void del_book(const char *title) {
    for (size_t i = 0; i < bc; i++) {
        if (strcmp(books[i].title, title) == 0) {
            free(books[i].title);
            for (size_t j = 0; j < books[i].ac; j++) free(books[i].authors[j]);
            free(books[i].authors);
            books[i] = books[--bc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256]; int year;
            if (sscanf(line, "%*s %255s %d", title, &year) == 2) add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(line, "%*s %255s %255s", title, name) == 2) {
                Book *b = find_book(title);
                if (b) add_author(b, name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(line, "%*s %255s %255s", title, name) == 2) {
                Book *b = find_book(title);
                if (b) rem_author(b, name);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) del_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                Book *b = find_book(title);
                if (b) {
                    for (size_t i = 0; i < b->ac; i++) printf("%s\n", b->authors[i]);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < bc; i++) printf("%s %d\n", books[i].title, books[i].year);
        }
        free(line);
    }
    for (size_t i = 0; i < bc; i++) {
        free(books[i].title);
        for (size_t j = 0; j < books[i].ac; j++) free(books[i].authors[j]);
        free(books[i].authors);
    }
    free(books);
    return 0;
}