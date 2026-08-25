// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int author_count;
    int author_capacity;
} Book;

Book *books = NULL;
int book_count = 0;
int book_capacity = 0;

int find_book(const char *title) {
    for (int i = 0; i < book_count; i++) {
        if (strcmp(books[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

void add_book(const char *title, const char *year) {
    if (find_book(title) != -1) return;
    if (book_count >= book_capacity) {
        book_capacity = book_capacity == 0 ? 4 : book_capacity * 2;
        books = realloc(books, book_capacity * sizeof(Book));
    }
    books[book_count].title = strdup(title);
    books[book_count].year = strdup(year);
    books[book_count].authors = NULL;
    books[book_count].author_count = 0;
    books[book_count].author_capacity = 0;
    book_count++;
}

void add_author(const char *title, const char *name) {
    int idx = find_book(title);
    if (idx == -1) return;
    Book *b = &books[idx];
    if (b->author_count >= b->author_capacity) {
        b->author_capacity = b->author_capacity == 0 ? 4 : b->author_capacity * 2;
        b->authors = realloc(b->authors, b->author_capacity * sizeof(char *));
    }
    b->authors[b->author_count] = strdup(name);
    b->author_count++;
}

void remove_author(const char *title, const char *name) {
    int idx = find_book(title);
    if (idx == -1) return;
    Book *b = &books[idx];
    for (int i = 0; i < b->author_count; i++) {
        if (strcmp(b->authors[i], name) == 0) {
            free(b->authors[i]);
            for (int j = i; j < b->author_count - 1; j++) {
                b->authors[j] = b->authors[j + 1];
            }
            b->author_count--;
            break;
        }
    }
}

void delete_book(const char *title) {
    int idx = find_book(title);
    if (idx == -1) return;
    free(books[idx].title);
    free(books[idx].year);
    for (int i = 0; i < books[idx].author_count; i++) {
        free(books[idx].authors[i]);
    }
    free(books[idx].authors);
    for (int i = idx; i < book_count - 1; i++) {
        books[i] = books[i + 1];
    }
    book_count--;
}

void find_book_p(const char *title) {
    int idx = find_book(title);
    if (idx == -1) return;
    for (int i = 0; i < books[idx].author_count; i++) {
        printf("%s%s", books[idx].authors[i], (i == books[idx].author_count - 1) ? "" : " ");
    }
    printf("\n");
}

void list_books(void) {
    for (int i = 0; i < book_count; i++) {
        printf("%s %s", books[i].title, books[i].year);
        for (int j = 0; j < books[i].author_count; j++) {
            printf(" %s", books[i].authors[j]);
        }
        printf("\n");
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = strtok(NULL, " ");
            char *year = strtok(NULL, " ");
            if (title && year) add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            if (title && name) add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            if (title && name) remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_book_p(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free(line);
    for (int i = 0; i < book_count; i++) {
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
