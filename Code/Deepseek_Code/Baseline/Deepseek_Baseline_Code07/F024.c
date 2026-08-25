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

Book *head = NULL;
Book *tail = NULL;

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
    b->title = malloc(strlen(title)+1);
    strcpy(b->title, title);
    b->year = year;
    b->authors = NULL;
    b->next = NULL;
    if (tail) tail->next = b;
    else head = b;
    tail = b;
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    a->name = malloc(strlen(name)+1);
    strcpy(a->name, name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL;
    Author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else b->authors = a->next;
            free(a->name);
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void delete_book(const char *title) {
    Book *prev = NULL;
    Book *b = head;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            if (b == tail) tail = prev;
            free(b->title);
            Author *a = b->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void find_authors(const char *title) {
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
        printf("%s\n", b->title);
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
            Author *anext = a->next;
            free(a->name);
            free(a);
            a = anext;
        }
        free(b);
        b = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = strtok(rest, " ");
            if (!title) continue;
            char *year_str = strtok(NULL, " ");
            int year = year_str ? atoi(year_str) : 0;
            add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = strtok(rest, " ");
            if (!title) continue;
            char *name = strtok(NULL, "");
            if (name) add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = strtok(rest, " ");
            if (!title) continue;
            char *name = strtok(NULL, "");
            if (name) remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = strtok(rest, " ");
            if (title) delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(rest, " ");
            if (title) find_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_all();
    return 0;
}