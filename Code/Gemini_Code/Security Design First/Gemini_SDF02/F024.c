/* F024.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    size_t a_count;
    size_t a_capacity;
} Book;

int main(void) {
    Book *books = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "BOOK") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int year = atoi(p);

            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Book *temp = realloc(books, new_cap * sizeof(Book));
                    if (!temp) break;
                    books = temp;
                    capacity = new_cap;
                }
                books[count].title = strdup(title);
                books[count].year = year;
                books[count].authors = NULL;
                books[count].a_count = 0;
                books[count].a_capacity = 0;
                if (books[count].title) count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    if (b->a_count >= b->a_capacity) {
                        size_t new_cap = b->a_capacity == 0 ? 4 : b->a_capacity * 2;
                        char **temp = realloc(b->authors, new_cap * sizeof(char *));
                        if (!temp) break;
                        b->authors = temp;
                        b->a_capacity = new_cap;
                    }
                    b->authors[b->a_count] = strdup(name);
                    if (b->authors[b->a_count]) b->a_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    Book *b = &books[i];
                    for (size_t j = 0; j < b->a_count; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            free(b->authors[j]);
                            for (size_t k = j; k < b->a_count - 1; k++) {
                                b->authors[k] = b->authors[k + 1];
                            }
                            b->a_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    for (size_t j = 0; j < books[i].a_count; j++) {
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
            char *title = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (size_t j = 0; j < books[i].a_count; j++) {
                        if (j > 0) printf(" ");
                        printf("%s", books[i].authors[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", books[i].title, books[i].year);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(books[i].title);
        for (size_t j = 0; j < books[i].a_count; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}
