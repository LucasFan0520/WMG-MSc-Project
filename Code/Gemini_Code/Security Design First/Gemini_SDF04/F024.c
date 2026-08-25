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
    int auth_cap;
} Book;

Book *catalog = NULL;
int book_count = 0;
int book_cap = 0;

int find_book(const char *title) {
    for (int i = 0; i < book_count; i++) {
        if (strcmp(catalog[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "BOOK") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *year = space2 + 1;
                    if (find_book(arg1) == -1) {
                        if (book_count >= book_cap) {
                            book_cap = book_cap == 0 ? 4 : book_cap * 2;
                            Book *tmp = realloc(catalog, book_cap * sizeof(Book));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            catalog = tmp;
                        }
                        catalog[book_count].title = strdup(arg1);
                        catalog[book_count].year = strdup(year);
                        catalog[book_count].authors = NULL;
                        catalog[book_count].auth_count = 0;
                        catalog[book_count].auth_cap = 0;
                        if (catalog[book_count].title && catalog[book_count].year) {
                            book_count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "AUTHOR") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *auth_name = space2 + 1;
                    int idx = find_book(arg1);
                    if (idx != -1) {
                        Book *b = &catalog[idx];
                        if (b->auth_count >= b->auth_cap) {
                            b->auth_cap = b->auth_cap == 0 ? 4 : b->auth_cap * 2;
                            char **tmp = realloc(b->authors, b->auth_cap * sizeof(char*));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            b->authors = tmp;
                        }
                        b->authors[b->auth_count] = strdup(auth_name);
                        if (b->authors[b->auth_count]) {
                            b->auth_count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *auth_name = space2 + 1;
                    int idx = find_book(arg1);
                    if (idx != -1) {
                        Book *b = &catalog[idx];
                        for (int i = 0; i < b->auth_count; i++) {
                            if (strcmp(b->authors[i], auth_name) == 0) {
                                free(b->authors[i]);
                                for (int j = i; j < b->auth_count - 1; j++) {
                                    b->authors[j] = b->authors[j + 1];
                                }
                                b->auth_count--;
                                break;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_book(arg1);
                if (idx != -1) {
                    free(catalog[idx].title);
                    free(catalog[idx].year);
                    for (int i = 0; i < catalog[idx].auth_count; i++) {
                        free(catalog[idx].authors[i]);
                    }
                    free(catalog[idx].authors);
                    for (int i = idx; i < book_count - 1; i++) {
                        catalog[i] = catalog[i + 1];
                    }
                    book_count--;
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                int idx = find_book(arg1);
                if (idx != -1) {
                    for (int i = 0; i < catalog[idx].auth_count; i++) {
                        if (i > 0) printf(", ");
                        printf("%s", catalog[idx].authors[i]);
                    }
                    printf("\n");
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < book_count; i++) {
                    printf("%s (%s)\n", catalog[i].title, catalog[i].year);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < book_count; i++) {
        free(catalog[i].title);
        free(catalog[i].year);
        for (int j = 0; j < catalog[i].auth_count; j++) {
            free(catalog[i].authors[j]);
        }
        free(catalog[i].authors);
    }
    free(catalog);
    return 0;
}