/* F024.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Book {
    char *title;
    int year;
    char **authors;
    size_t acount, acap;
} Book;

static Book *books = NULL;
static size_t bcount = 0, bcap = 0;

static Book *find_book(const char *title) {
    for (size_t i = 0; i < bcount; i++) {
        if (strcmp(books[i].title, title) == 0)
            return &books[i];
    }
    return NULL;
}

static void add_book(const char *title, int year) {
    if (bcount == bcap) {
        bcap = bcap ? bcap * 2 : 4;
        books = realloc(books, bcap * sizeof(Book));
        if (!books) { perror("realloc"); exit(1); }
    }
    books[bcount].title = strdup(title);
    books[bcount].year = year;
    books[bcount].authors = NULL;
    books[bcount].acount = books[bcount].acap = 0;
    bcount++;
}

static void add_author(Book *b, const char *name) {
    if (b->acount == b->acap) {
        b->acap = b->acap ? b->acap * 2 : 4;
        b->authors = realloc(b->authors, b->acap * sizeof(char *));
        if (!b->authors) { perror("realloc"); exit(1); }
    }
    b->authors[b->acount] = strdup(name);
    b->acount++;
}

static void remove_author(Book *b, const char *name) {
    for (size_t i = 0; i < b->acount; i++) {
        if (strcmp(b->authors[i], name) == 0) {
            free(b->authors[i]);
            for (size_t j = i; j < b->acount - 1; j++)
                b->authors[j] = b->authors[j + 1];
            b->acount--;
            return;
        }
    }
}

static void delete_book(const char *title) {
    for (size_t i = 0; i < bcount; i++) {
        if (strcmp(books[i].title, title) == 0) {
            free(books[i].title);
            for (size_t j = 0; j < books[i].acount; j++) free(books[i].authors[j]);
            free(books[i].authors);
            for (size_t j = i; j < bcount - 1; j++)
                books[j] = books[j + 1];
            bcount--;
            return;
        }
    }
}

static void find_book_authors(const char *title) {
    Book *b = find_book(title);
    if (!b) return;
    for (size_t i = 0; i < b->acount; i++) {
        printf("%s\n", b->authors[i]);
    }
}

static void list_books(void) {
    for (size_t i = 0; i < bcount; i++) {
        printf("%s %d\n", books[i].title, books[i].year);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *yearstr = strtok(NULL, " ");
            if (!yearstr) continue;
            int year = atoi(yearstr);
            if (find_book(title)) {
                Book *b = find_book(title);
                b->year = year;
            } else {
                add_book(title, year);
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *name = strtok(NULL, " ");
            if (!name) continue;
            Book *b = find_book(title);
            if (b) add_author(b, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *name = strtok(NULL, " ");
            if (!name) continue;
            Book *b = find_book(title);
            if (b) remove_author(b, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    for (size_t i = 0; i < bcount; i++) {
        free(books[i].title);
        for (size_t j = 0; j < books[i].acount; j++) free(books[i].authors[j]);
        free(books[i].authors);
    }
    free(books);
    return 0;
}