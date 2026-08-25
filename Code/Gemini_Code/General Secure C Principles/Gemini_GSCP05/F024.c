// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    int author_count;
    int author_capacity;
} Book;

int main(void) {
    Book *books = NULL;
    int count = 0;
    int capacity = 0;
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
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Book *new_b = realloc(books, capacity * sizeof(Book));
                        if (new_b) books = new_b;
                    }
                    if (count < capacity) {
                        books[count].title = strdup(title);
                        books[count].year = strdup(year);
                        books[count].authors = NULL;
                        books[count].author_count = 0;
                        books[count].author_capacity = 0;
                        count++;
                    }
                }
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
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        Book *b = &books[i];
                        if (b->author_count >= b->author_capacity) {
                            b->author_capacity = b->author_capacity == 0 ? 4 : b->author_capacity * 2;
                            char **new_a = realloc(b->authors, b->author_capacity * sizeof(char *));
                            if (new_a) b->authors = new_a;
                        }
                        if (b->author_count < b->author_capacity) {
                            b->authors[b->author_count] = strdup(name);
                            b->author_count++;
                        }
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
                for (int i = 0; i < count; i++) {
                    if (strcmp(books[i].title, title) == 0) {
                        Book *b = &books[i];
                        for (int j = 0; j < b->author_count; j++) {
                            if (strcmp(b->authors[j], name) == 0) {
                                free(b->authors[j]);
                                for (int k = j; k < b->author_count - 1; k++) {
                                    b->authors[k] = b->authors[k + 1];
                                }
                                b->author_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    free(books[i].title);
                    free(books[i].year);
                    for (int j = 0; j < books[i].author_count; j++) {
                        free(books[i].authors[j]);
                    }
                    free(books[i].authors);
                    for (int j = i; j < count - 1; j++) {
                        books[j] = books[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    printf("Book: %s (%s) Authors:", books[i].title, books[i].year);
                    for (int j = 0; j < books[i].author_count; j++) {
                        printf(" %s", books[i].authors[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s (%s)\n", books[i].title, books[i].year);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(books[i].title);
        free(books[i].year);
        for (int j = 0; j < books[i].author_count; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}
