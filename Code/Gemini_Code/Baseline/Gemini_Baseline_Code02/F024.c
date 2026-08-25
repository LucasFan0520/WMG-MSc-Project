// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    int year;
    char **authors;
    int author_count;
} Book;

int main() {
    Book *books = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s (%d)\n", books[i].title, books[i].year);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                int year = atoi(p2 + 1);
                books = realloc(books, sizeof(Book) * (count + 1));
                books[count].title = strdup(title);
                books[count].year = year;
                books[count].authors = NULL;
                books[count].author_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        int ac = books[i].author_count;
                        books[i].authors = realloc(books[i].authors, sizeof(char*) * (ac + 1));
                        books[i].authors[ac] = strdup(name);
                        books[i].author_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = p2 + 1;
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
                                    books[i].authors = realloc(books[i].authors, sizeof(char*) * books[i].author_count);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(books[i].title, args) == 0) {
                    free(books[i].title);
                    for (int j = 0; j < books[i].author_count; j++) free(books[i].authors[j]);
                    free(books[i].authors);
                    for (int j = i; j < count - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    count--;
                    if (count == 0) { free(books); books = NULL; }
                    else { books = realloc(books, sizeof(Book) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(books[i].title, args) == 0) {
                    for (int j = 0; j < books[i].author_count; j++) {
                        printf("%s%s", books[i].authors[j], (j == books[i].author_count - 1) ? "" : ", ");
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].author_count; j++) free(books[i].authors[j]);
        free(books[i].authors);
    }
    free(books);
    return 0;
}
