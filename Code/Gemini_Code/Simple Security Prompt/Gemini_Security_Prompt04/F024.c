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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    int a_count;
    int a_cap;
} Book;

int main(void) {
    Book *books = NULL;
    int b_count = 0;
    int b_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *year = p + 1;
                while (*year == ' ') year++;
                if (b_count >= b_cap) {
                    b_cap = b_cap == 0 ? 16 : b_cap * 2;
                    books = realloc(books, b_cap * sizeof(Book));
                }
                books[b_count].title = strdup(title);
                books[b_count].year = strdup(year);
                books[b_count].authors = NULL;
                books[b_count].a_count = 0;
                books[b_count].a_cap = 0;
                b_count++;
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *name = p + 1;
                while (*name == ' ') name++;
                for (int i = 0; i < b_count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        Book *b = &books[i];
                        if (b->a_count >= b->a_cap) {
                            b->a_cap = b->a_cap == 0 ? 16 : b->a_cap * 2;
                            b->authors = realloc(b->authors, b->a_cap * sizeof(char *));
                        }
                        b->authors[b->a_count] = strdup(name);
                        b->a_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *name = p + 1;
                while (*name == ' ') name++;
                for (int i = 0; i < b_count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        Book *b = &books[i];
                        int idx = -1;
                        for (int j = 0; j < b->a_count; j++) {
                            if (strcmp(b->authors[j], name) == 0) {
                                idx = j;
                                break;
                            }
                        }
                        if (idx != -1) {
                            free(b->authors[idx]);
                            for (int j = idx; j < b->a_count - 1; j++) {
                                b->authors[j] = b->authors[j + 1];
                            }
                            b->a_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            int idx = -1;
            for (int i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(books[idx].title);
                free(books[idx].year);
                for (int j = 0; j < books[idx].a_count; j++) free(books[idx].authors[j]);
                free(books[idx].authors);
                for (int i = idx; i < b_count - 1; i++) {
                    books[i] = books[i + 1];
                }
                b_count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (int j = 0; j < books[i].a_count; j++) {
                        printf("%s\n", books[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < b_count; i++) {
                printf("%s (%s)\n", books[i].title, books[i].year);
            }
        }
        free(line);
    }
    for (int i = 0; i < b_count; i++) {
        free(books[i].title);
        free(books[i].year);
        for (int j = 0; j < books[i].a_count; j++) free(books[i].authors[j]);
        free(books[i].authors);
    }
    free(books);
    return 0;
}
