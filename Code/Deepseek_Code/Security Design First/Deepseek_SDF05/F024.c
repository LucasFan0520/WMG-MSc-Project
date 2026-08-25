// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

Author *find_author(Book *b, const char *name) {
    Author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void book_add(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    b->title = safe_strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void author_add(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    if (find_author(b, name)) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
    a->name = safe_strdup(name);
    a->next = b->authors;
    b->authors = a;
}

void author_remove(const char *title, const char *name) {
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

void book_delete(const char *title) {
    Book *prev = NULL, *cur = books;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else books = cur->next;
            free(cur->title);
            Author *a = cur->authors;
            while (a) {
                Author *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
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
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

void free_all(void) {
    Book *b = books;
    while (b) {
        Book *next_b = b->next;
        free(b->title);
        Author *a = b->authors;
        while (a) {
            Author *next_a = a->next;
            free(a->name);
            free(a);
            a = next_a;
        }
        free(b);
        b = next_b;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "BOOK") == 0 && n >= 3) {
            int year = atoi(arg2);
            book_add(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0 && n >= 3) {
            author_add(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n >= 3) {
            author_remove(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            book_delete(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            book_find(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            book_list();
        }
    }
    free_all();
    return 0;
}