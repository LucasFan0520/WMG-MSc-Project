// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    int author_count;
} Book;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

int main(void) {
    Book *books = NULL;
    int book_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int year = atoi(p);
            books = realloc(books, (book_count + 1) * sizeof(Book));
            books[book_count].title = strdup(title);
            books[book_count].year = year;
            books[book_count].authors = NULL;
            books[book_count].author_count = 0;
            book_count++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < book_count; i++) {
                if (books[i].title && strcmp(books[i].title, title) == 0) {
                    int ac = books[i].author_count;
                    books[i].authors = realloc(books[i].authors, (ac + 1) * sizeof(char *));
                    books[i].authors[ac] = strdup(name);
                    books[i].author_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < book_count; i++) {
                if (books[i].title && strcmp(books[i].title, title) == 0) {
                    for (int j = 0; j < books[i].author_count; j++) {
                        if (strcmp(books[i].authors[j], name) == 0) {
                            free(books[i].authors[j]);
                            for (int k = j; k < books[i].author_count - 1; k++) {
                                books[i].authors[k] = books[i].authors[k + 1];
                            }
                            books[i].author_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < book_count; i++) {
                if (books[i].title && strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    for (int j = 0; j < books[i].author_count; j++) {
                        free(books[i].authors[j]);
                    }
                    free(books[i].authors);
                    books[i].title = NULL;
                    books[i].authors = NULL;
                    books[i].author_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < book_count; i++) {
                if (books[i].title && strcmp(books[i].title, title) == 0) {
                    for (int j = 0; j < books[i].author_count; j++) {
                        printf("%s\n", books[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < book_count; i++) {
                if (books[i].title) {
                    printf("%s (%d)\n", books[i].title, books[i].year);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < book_count; i++) {
        if (books[i].title) {
            free(books[i].title);
            for (int j = 0; j < books[i].author_count; j++) {
                free(books[i].authors[j]);
            }
            free(books[i].authors);
        }
    }
    free(books);
    return 0;
}
