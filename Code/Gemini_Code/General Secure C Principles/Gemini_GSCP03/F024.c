// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

typedef struct {
    char *title;
    char *year;
    char **authors;
    int auth_count;
    int auth_cap;
} Book;

int main() {
    Book *books = NULL;
    int b_count = 0;
    int b_cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < b_count; i++) {
                printf("%s %s\n", books[i].title, books[i].year);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "BOOK") == 0) {
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
            if (b_count >= b_cap) {
                b_cap = b_cap == 0 ? 16 : b_cap * 2;
                Book *nb = realloc(books, b_cap * sizeof(Book));
                if (nb) books = nb;
            }
            books[b_count].title = strdup(title);
            books[b_count].year = strdup(year);
            books[b_count].authors = NULL;
            books[b_count].auth_count = 0;
            books[b_count].auth_cap = 0;
            b_count++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
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
            for (int i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    if (b->auth_count >= b->auth_cap) {
                        b->auth_cap = b->auth_cap == 0 ? 16 : b->auth_cap * 2;
                        char **nb = realloc(b->authors, b->auth_cap * sizeof(char *));
                        if (nb) b->authors = nb;
                    }
                    b->authors[b->auth_count] = strdup(name);
                    b->auth_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
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
            for (int i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    for (int j = 0; j < b->auth_count; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            free(b->authors[j]);
                            for (int k = j; k < b->auth_count - 1; k++) {
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
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    free(books[i].year);
                    for (int j = 0; j < books[i].auth_count; j++) {
                        free(books[i].authors[j]);
                    }
                    free(books[i].authors);
                    for (int j = i; j < b_count - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    b_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (int j = 0; j < books[i].auth_count; j++) {
                        printf("%s%s", books[i].authors[j], (j == books[i].auth_count - 1) ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < b_count; i++) {
        free(books[i].title);
        free(books[i].year);
        for (int j = 0; j < books[i].auth_count; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}
