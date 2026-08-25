/* F024.c */
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

Book *find_book(const char *title) {
    Book *b = head;
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

void book_add(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    if (!b) return;
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = head;
    head = b;
}

void author_add(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    if (find_author(b, name)) return;
    Author *a = malloc(sizeof(Author));
    if (!a) return;
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
    Book *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->title);
            Author *a = curr->authors;
            while (a) {
                Author *tmp = a;
                a = a->next;
                free(tmp->name);
                free(tmp);
            }
            free(curr);
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
    int first = 1;
    while (a) {
        if (!first) printf(" ");
        printf("%s", a->name);
        first = 0;
        a = a->next;
    }
    printf("\n");
}

void book_list(void) {
    Book *b = head;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

void free_all(void) {
    Book *b = head;
    while (b) {
        Book *tmp = b;
        b = b->next;
        free(tmp->title);
        Author *a = tmp->authors;
        while (a) {
            Author *tmp_a = a;
            a = a->next;
            free(tmp_a->name);
            free(tmp_a);
        }
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year;
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", title, &year) == 2) {
                book_add(title, year);
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(line + 7, "%255s %255s", title, name) == 2) {
                author_add(title, name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(line + 13, "%255s %255s", title, name) == 2) {
                author_remove(title, name);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            book_delete(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            book_find(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            book_list();
        }
    }
    free_all();
    return 0;
}