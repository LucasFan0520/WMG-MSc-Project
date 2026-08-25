// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t author_count;
} Book;

int main(void) {
    Book *books = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
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
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *title != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Book *new_books = realloc(books, capacity * sizeof(Book));
                    if (new_books) books = new_books;
                }
                if (count < capacity) {
                    books[count].title = strdup(title);
                    books[count].year = strdup(year);
                    books[count].authors = NULL;
                    books[count].author_count = 0;
                    count++;
                }
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    books[i].authors = realloc(books[i].authors, (books[i].author_count + 1) * sizeof(char *));
                    books[i].authors[books[i].author_count++] = strdup(name);
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
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
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
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    printf("%s (%s):", books[i].title, books[i].year);
                    for (size_t k = 0; k < books[i].author_count; k++) {
                        printf(" %s", books[i].authors[k]);
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
        free(line);
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
    return 0;
}
