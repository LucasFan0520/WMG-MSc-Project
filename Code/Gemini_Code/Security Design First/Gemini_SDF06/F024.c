// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t author_count;
    size_t author_capacity;
} Book;

int main(void) {
    Book *books = NULL;
    size_t book_count = 0;
    size_t book_capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *year = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*title == '\0' || *year == '\0') continue;

            if (book_count >= book_capacity) {
                size_t new_cap = book_capacity == 0 ? 4 : book_capacity * 2;
                Book *new_books = realloc(books, new_cap * sizeof(Book));
                if (!new_books) continue;
                books = new_books;
                book_capacity = new_cap;
            }
            char *ttl = strdup(title);
            char *yr = strdup(year);
            if (ttl && yr) {
                books[book_count].title = ttl;
                books[book_count].year = yr;
                books[book_count].authors = NULL;
                books[book_count].author_count = 0;
                books[book_count].author_capacity = 0;
                book_count++;
            } else {
                free(ttl);
                free(yr);
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*title == '\0' || *name == '\0') continue;

            for (size_t i = 0; i < book_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    if (b->author_count >= b->author_capacity) {
                        size_t new_cap = b->author_capacity == 0 ? 4 : b->author_capacity * 2;
                        char **new_auths = realloc(b->authors, new_cap * sizeof(char *));
                        if (!new_auths) break;
                        b->authors = new_auths;
                        b->author_capacity = new_cap;
                    }
                    char *nm = strdup(name);
                    if (nm) {
                        b->authors[b->author_count] = nm;
                        b->author_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*title == '\0' || *name == '\0') continue;

            for (size_t i = 0; i < book_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    for (size_t j = 0; j < b->author_count; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            free(b->authors[j]);
                            for (size_t k = j; k < b->author_count - 1; k++) {
                                b->authors[k] = b->authors[k + 1];
                            }
                            b->author_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*title == '\0') continue;

            for (size_t i = 0; i < book_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    free(books[i].year);
                    for (size_t j = 0; j < books[i].author_count; j++) {
                        free(books[i].authors[j]);
                    }
                    free(books[i].authors);
                    for (size_t j = i; j < book_count - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    book_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*title == '\0') continue;

            for (size_t i = 0; i < book_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    printf("Book: %s Year: %s Authors:", books[i].title, books[i].year);
                    for (size_t j = 0; j < books[i].author_count; j++) {
                        printf(" %s", books[i].authors[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < book_count; i++) {
                printf("%s (%s)\n", books[i].title, books[i].year);
            }
        }
    }

    for (size_t i = 0; i < book_count; i++) {
        free(books[i].title);
        free(books[i].year);
        for (size_t j = 0; j < books[i].author_count; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    free(line);
    return 0;
}
