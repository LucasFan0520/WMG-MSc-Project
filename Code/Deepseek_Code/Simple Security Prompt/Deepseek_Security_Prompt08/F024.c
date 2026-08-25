// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Author {
    char *name;
    struct Author *next;
} Author;

typedef struct Book {
    char *title;
    int year;
    Author *authors;
    struct Book *next;
} Book;

Book *head = NULL, *tail = NULL;

Book *find_book(const char *title) {
    Book *b = head;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = malloc(strlen(title) + 1);
    strcpy(b->title, title);
    b->year = year;
    b->authors = NULL;
    b->next = NULL;
    if (tail) {
        tail->next = b;
        tail = b;
    } else {
        head = tail = b;
    }
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    a->name = malloc(strlen(name) + 1);
    strcpy(a->name, name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL, *curr = b->authors;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else b->authors = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_book(const char *title) {
    Book *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->title);
            Author *a = curr->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_book_authors(const char *title) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void list_books(void) {
    Book *b = head;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

void free_all(void) {
    Book *b = head;
    while (b) {
        Book *next = b->next;
        free(b->title);
        Author *a = b->authors;
        while (a) {
            Author *nexta = a->next;
            free(a->name);
            free(a);
            a = nexta;
        }
        free(b);
        b = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = strtok(NULL, " ");
            char *year = strtok(NULL, " ");
            if (title && year) add_book(title, atoi(year));
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
            if (title) find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free(line);
    free_all();
    return 0;
}