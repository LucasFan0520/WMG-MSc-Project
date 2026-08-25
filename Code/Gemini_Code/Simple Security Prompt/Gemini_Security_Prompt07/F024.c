// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int author_count;
} Book;

int main() {
    Book *books = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = strtok(NULL, " ");
            char *year = strtok(NULL, " ");
            if (title && year) {
                books = realloc(books, (count + 1) * sizeof(Book));
                books[count].title = strdup(title);
                books[count].year = strdup(year);
                books[count].authors = NULL;
                books[count].author_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            if (title && name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        books[i].authors = realloc(books[i].authors, (books[i].author_count + 1) * sizeof(char *));
                        books[i].authors[books[i].author_count] = strdup(name);
                        books[i].author_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            if (title && name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        for (int j = 0; j < books[i].author_count; j++) {
                            if (strcmp(books[i].authors[j], name) == 0) {
                                free(books[i].authors[j]);
                                for (int k = j; k < books[i].author_count - 1; k++) {
                                    books[i].authors[k] = books[i].authors[k + 1];
                                }
                                books[i].author_count--;
                                if (books[i].author_count == 0) {
                                    free(books[i].authors);
                                    books[i].authors = NULL;
                                } else {
                                    books[i].authors = realloc(books[i].authors, books[i].author_count * sizeof(char *));
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        free(books[i].title);
                        free(books[i].year);
                        for (int j = 0; j < books[i].author_count; j++) {
                            free(books[i].authors[j]);
                        }
                        free(books[i].authors);
                        for (int j = i; j < count - 1; j++) {
                            books[j] = books[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(books);
                    books = NULL;
                } else {
                    books = realloc(books, count * sizeof(Book));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        for (int j = 0; j < books[i].author_count; j++) {
                            printf("%s\n", books[i].authors[j]);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s (%s)\n", books[i].title, books[i].year);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(books[i].title);
        free(books[i].year);
        for (int j = 0; j < books[i].author_count; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}
