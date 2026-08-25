// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    int acount;
} Book;

int main(void) {
    Book *books = NULL;
    int bcount = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char title[256];
            int year;
            if (sscanf(line + 5, "%255s %d", title, &year) == 2) {
                int found = 0;
                for (int i = 0; i < bcount; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    Book *tmp = realloc(books, (bcount + 1) * sizeof(Book));
                    if (tmp) {
                        books = tmp;
                        books[bcount].title = strdup(title);
                        books[bcount].year = year;
                        books[bcount].authors = NULL;
                        books[bcount].acount = 0;
                        bcount++;
                    }
                }
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char title[256], author[256];
            if (sscanf(line + 7, "%255s %255s", title, author) == 2) {
                for (int i = 0; i < bcount; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        char **atmp = realloc(books[i].authors, (books[i].acount + 1) * sizeof(char *));
                        if (atmp) {
                            books[i].authors = atmp;
                            books[i].authors[books[i].acount] = strdup(author);
                            books[i].acount++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char title[256], author[256];
            if (sscanf(line + 13, "%255s %255s", title, author) == 2) {
                for (int i = 0; i < bcount; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        for (int j = 0; j < books[i].acount; j++) {
                            if (strcmp(books[i].authors[j], author) == 0) {
                                free(books[i].authors[j]);
                                for (int k = j; k < books[i].acount - 1; k++) {
                                    books[i].authors[k] = books[i].authors[k + 1];
                                }
                                books[i].acount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (int i = 0; i < bcount; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    for (int j = 0; j < books[i].acount; j++) {
                        free(books[i].authors[j]);
                    }
                    free(books[i].authors);
                    for (int j = i; j < bcount - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    bcount--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < bcount; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (int j = 0; j < books[i].acount; j++) {
                        printf("%s\n", books[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < bcount; i++) {
                printf("%s (%d)\n", books[i].title, books[i].year);
            }
        }
    }
    free(line);
    for (int i = 0; i < bcount; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].acount; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}
