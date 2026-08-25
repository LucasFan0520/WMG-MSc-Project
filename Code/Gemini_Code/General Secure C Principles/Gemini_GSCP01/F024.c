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
} BibliographyBook;

int main(void) {
    BibliographyBook *books = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *year = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                BibliographyBook *temp = realloc(books, (count + 1) * sizeof(BibliographyBook));
                if (temp) {
                    books = temp;
                    books[count].title = strdup(title);
                    books[count].year = strdup(year);
                    books[count].authors = NULL;
                    books[count].author_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    char **temp = realloc(books[i].authors, (books[i].author_count + 1) * sizeof(char *));
                    if (temp) {
                        books[i].authors = temp;
                        books[i].authors[books[i].author_count] = strdup(name);
                        books[i].author_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (size_t k = 0; k < books[i].author_count; k++) {
                        if (strcmp(books[i].authors[k], name) == 0) {
                            free(books[i].authors[k]);
                            for (size_t j = k; j < books[i].author_count - 1; j++) {
                                books[i].authors[j] = books[i].authors[j + 1];
                            }
                            books[i].author_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    free(books[i].year);
                    for (size_t k = 0; k < books[i].author_count; k++) {
                        free(books[i].authors[k]);
                    }
                    free(books[i].authors);
                    for (size_t j = i; j < count - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (size_t k = 0; k < books[i].author_count; k++) {
                        if (k > 0) printf(", ");
                        printf("%s", books[i].authors[k]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s (%s)\n", books[i].title, books[i].year);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(books[i].title);
        free(books[i].year);
        for (size_t k = 0; k < books[i].author_count; k++) {
            free(books[i].authors[k]);
        }
        free(books[i].authors);
    }
    free(books);
    free(line);
    return 0;
}
