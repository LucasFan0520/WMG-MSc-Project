// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t acount;
    size_t acap;
} Book;

int main() {
    Book *books = NULL;
    size_t bcount = 0;
    size_t bcap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *p = line + 5;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *title = p;
                char *year = s1 + 1;
                if (bcount >= bcap) {
                    bcap = bcap == 0 ? 4 : bcap * 2;
                    books = realloc(books, bcap * sizeof(Book));
                }
                books[bcount].title = strdup(title);
                books[bcount].year = strdup(year);
                books[bcount].authors = NULL;
                books[bcount].acount = 0;
                books[bcount].acap = 0;
                bcount++;
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p = line + 7;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *title = p;
                char *name = s1 + 1;
                for (size_t i = 0; i < bcount; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        Book *b = &books[i];
                        if (b->acount >= b->acap) {
                            b->acap = b->acap == 0 ? 4 : b->acap * 2;
                            b->authors = realloc(b->authors, b->acap * sizeof(char *));
                        }
                        b->authors[b->acount++] = strdup(name);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p = line + 13;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *title = p;
                char *name = s1 + 1;
                for (size_t i = 0; i < bcount; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        Book *b = &books[i];
                        for (size_t j = 0; j < b->acount; j++) {
                            if (strcmp(b->authors[j], name) == 0) {
                                free(b->authors[j]);
                                for (size_t k = j; k < b->acount - 1; k++) {
                                    b->authors[k] = b->authors[k + 1];
                                }
                                b->acount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            for (size_t i = 0; i < bcount; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    free(books[i].year);
                    for (size_t j = 0; j < books[i].acount; j++) {
                        free(books[i].authors[j]);
                    }
                    free(books[i].authors);
                    for (size_t j = i; j < bcount - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    bcount--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < bcount; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (size_t j = 0; j < books[i].acount; j++) {
                        printf("%s\n", books[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < bcount; i++) {
                printf("%s %s\n", books[i].title, books[i].year);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < bcount; i++) {
        free(books[i].title);
        free(books[i].year);
        for (size_t j = 0; j < books[i].acount; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}
