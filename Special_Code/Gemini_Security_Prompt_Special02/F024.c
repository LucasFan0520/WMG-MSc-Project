// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t auth_count;
    size_t auth_cap;
} Book;

int main() {
    Book *books = NULL;
    size_t b_count = 0;
    size_t b_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *title = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *arg2 = p;
        while (*p && *p != ' ') p++;
        if (*p) *p = '\0';
        if (strcmp(cmd, "BOOK") == 0) {
            if (b_count >= b_cap) {
                b_cap = b_cap == 0 ? 4 : b_cap * 2;
                books = realloc(books, b_cap * sizeof(Book));
            }
            books[b_count].title = mystrdup(title);
            books[b_count].year = mystrdup(arg2);
            books[b_count].authors = NULL;
            books[b_count].auth_count = 0;
            books[b_count].auth_cap = 0;
            b_count++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    if (books[i].auth_count >= books[i].auth_cap) {
                        books[i].auth_cap = books[i].auth_cap == 0 ? 4 : books[i].auth_cap * 2;
                        books[i].authors = realloc(books[i].authors, books[i].auth_cap * sizeof(char *));
                    }
                    books[i].authors[books[i].auth_count++] = mystrdup(arg2);
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (size_t j = 0; j < books[i].auth_count; j++) {
                        if (strcmp(books[i].authors[j], arg2) == 0) {
                            free(books[i].authors[j]);
                            for (size_t k = j; k < books[i].auth_count - 1; k++) {
                                books[i].authors[k] = books[i].authors[k + 1];
                            }
                            books[i].auth_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    free(books[i].year);
                    for (size_t j = 0; j < books[i].auth_count; j++) {
                        free(books[i].authors[j]);
                    }
                    free(books[i].authors);
                    for (size_t j = i; j < b_count - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    b_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    printf("Title: %s Year: %s Authors:", books[i].title, books[i].year);
                    for (size_t j = 0; j < books[i].auth_count; j++) {
                        printf(" %s", books[i].authors[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < b_count; i++) {
                printf("%s\n", books[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < b_count; i++) {
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
