/* F024.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct author {
    char *name;
    struct author *next;
} author_t;

typedef struct book {
    char *title;
    int year;
    author_t *authors;
    struct book *next;
} book_t;

book_t *books = NULL;

book_t *find_book(const char *title) {
    book_t *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void book_cmd(const char *title, int year) {
    if (find_book(title)) return;
    book_t *b = malloc(sizeof(book_t));
    if (!b) return;
    b->title = strdup(title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
    if (!b->title) { free(b); }
}

void author_cmd(const char *title, const char *name) {
    book_t *b = find_book(title);
    if (!b) return;
    author_t *a = malloc(sizeof(author_t));
    if (!a) return;
    a->name = strdup(name);
    a->next = b->authors;
    b->authors = a;
    if (!a->name) { free(a); }
}

void removeauthor_cmd(const char *title, const char *name) {
    book_t *b = find_book(title);
    if (!b) return;
    author_t *prev = NULL, *a = b->authors;
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

void delete_cmd(const char *title) {
    book_t *prev = NULL, *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            free(b->title);
            author_t *a = b->authors;
            while (a) {
                author_t *nxt = a->next;
                free(a->name);
                free(a);
                a = nxt;
            }
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void find_cmd(const char *title) {
    book_t *b = find_book(title);
    if (!b) {
        printf("NOT_FOUND\n");
        return;
    }
    author_t *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void list_cmd(void) {
    book_t *b = books;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

void free_all(void) {
    book_t *b = books;
    while (b) {
        book_t *nxt = b->next;
        free(b->title);
        author_t *a = b->authors;
        while (a) {
            author_t *n = a->next;
            free(a->name);
            free(a);
            a = n;
        }
        free(b);
        b = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[256], b[256];
        int year;
        if (sscanf(line, "%19s %255s %d %255s", cmd, a, &year, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "BOOK") == 0) {
                if (sscanf(line, "%*s %255s %d", a, &year) == 2)
                    book_cmd(a, year);
            } else if (strcmp(cmd, "AUTHOR") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    author_cmd(a, b);
            } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    removeauthor_cmd(a, b);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_cmd(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_cmd(a);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}