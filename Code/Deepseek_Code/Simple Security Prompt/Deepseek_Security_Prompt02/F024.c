/* F024.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_authors(Author *a) {
    while (a) {
        Author *tmp = a;
        a = a->next;
        free(tmp->name);
        free(tmp);
    }
}

void free_book(Book *b) {
    free(b->title);
    free_authors(b->authors);
    free(b);
}

void book_add(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void author_add(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *a = malloc(sizeof(Author));
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void author_remove(const char *title, const char *name) {
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

void book_delete(const char *title) {
    Book *prev = NULL, *curr = books;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else books = curr->next;
            free_book(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void book_find(const char *title) {
    Book *b = find_book(title);
    if (!b) {
        printf("NOT_FOUND\n");
        return;
    }
    Author *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void book_list(void) {
    Book *b = books;
    while (b) {
        printf("%s\n", b->title);
        b = b->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char title[256], name[256];
    int year;

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "BOOK") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", title, &year) == 2) book_add(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", title, name) == 2) author_add(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *p = line + 12;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", title, name) == 2) author_remove(title, name);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) book_delete(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) book_find(title);
        } else if (strcmp(cmd, "LIST") == 0) {
            book_list();
        }
    }

    while (books) {
        Book *tmp = books;
        books = books->next;
        free_book(tmp);
    }
    return 0;
}