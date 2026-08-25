// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int auth_count;
} Book;

int main(void) {
    Book *books = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *title = p;
                char *year = space + 1;
                Book *tmp = realloc(books, sizeof(Book) * (count + 1));
                if (tmp) {
                    books = tmp;
                    books[count].title = strdup(title);
                    books[count].year = strdup(year);
                    books[count].authors = NULL;
                    books[count].auth_count = 0;
                    if (books[count].title && books[count].year) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *title = p;
                char *name = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        char **atmp = realloc(books[i].authors, sizeof(char *) * (books[i].auth_count + 1));
                        if (atmp) {
                            books[i].authors = atmp;
                            books[i].authors[books[i].auth_count] = strdup(name);
                            if (books[i].authors[books[i].auth_count]) {
                                books[i].auth_count++;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p = line + 13;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *title = p;
                char *name = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        int a_idx = -1;
                        for (int j = 0; j < books[i].auth_count; j++) {
                            if (strcmp(books[i].authors[j], name) == 0) {
                                a_idx = j;
                                break;
                            }
                        }
                        if (a_idx != -1) {
                            free(books[i].authors[a_idx]);
                            for (int j = a_idx; j < books[i].auth_count - 1; j++) {
                                books[i].authors[j] = books[i].authors[j + 1];
                            }
                            books[i].auth_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(books[idx].title);
                free(books[idx].year);
                for (int j = 0; j < books[idx].auth_count; j++) {
                    free(books[idx].authors[j]);
                }
                free(books[idx].authors);
                for (int i = idx; i < count - 1; i++) {
                    books[i] = books[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (int j = 0; j < books[i].auth_count; j++) {
                        printf("%s\n", books[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s (%s)\n", books[i].title, books[i].year);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(books[i].title);
        free(books[i].year);
        for (int j = 0; j < books[i].auth_count; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    free(line);
    return 0;
}
