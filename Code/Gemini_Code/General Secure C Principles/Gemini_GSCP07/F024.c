// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t auth_count;
    size_t auth_capacity;
} Book;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Book *books = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *year = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Book *nb = realloc(books, capacity * sizeof(Book));
                if (!nb) break;
                books = nb;
            }
            books[count].title = strdup(title);
            books[count].year = strdup(year);
            books[count].authors = NULL;
            books[count].auth_count = 0;
            books[count].auth_capacity = 0;
            count++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    if (b->auth_count >= b->auth_capacity) {
                        b->auth_capacity = b->auth_capacity == 0 ? 4 : b->auth_capacity * 2;
                        char **na = realloc(b->authors, b->auth_capacity * sizeof(char *));
                        if (!na) break;
                        b->authors = na;
                    }
                    b->authors[b->auth_count++] = strdup(name);
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    for (size_t j = 0; j < b->auth_count; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            free(b->authors[j]);
                            for (size_t k = j; k < b->auth_count - 1; k++) {
                                b->authors[k] = b->authors[k + 1];
                            }
                            b->auth_count--;
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    free(books[i].year);
                    for (size_t j = 0; j < books[i].auth_count; j++) {
                        free(books[i].authors[j]);
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
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (size_t j = 0; j < books[i].auth_count; j++) {
                        if (j > 0) printf(", ");
                        printf("%s", books[i].authors[j]);
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
        for (size_t j = 0; j < books[i].auth_count; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}
