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

Book *books = NULL;

Book *find_book(const char *title) {
    Book *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return;
        a = a->next;
    }
    Author *new = malloc(sizeof(Author));
    if (!new) return;
    new->name = strdup(name);
    new->next = b->authors;
    b->authors = new;
}

void remove_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *prev = NULL, *cur = b->authors;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else b->authors = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_book(const char *title) {
    Book *prev = NULL, *cur = books;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else books = cur->next;
            Author *a = cur->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(cur->title);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
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
    Book *b = books;
    while (b) {
        printf("%s (%d)\n", b->title, b->year);
        b = b->next;
    }
}

void free_all(void) {
    Book *b = books;
    while (b) {
        Book *next = b->next;
        Author *a = b->authors;
        while (a) {
            Author *anext = a->next;
            free(a->name);
            free(a);
            a = anext;
        }
        free(b->title);
        free(b);
        b = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            int year;
            if (sscanf(space + 1, "%d", &year) != 1) continue;
            add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = space + 1;
            while (*name == ' ') name++;
            add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = space + 1;
            while (*name == ' ') name++;
            remove_author(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            delete_book(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            find_book_authors(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    free_all();
    return 0;
}