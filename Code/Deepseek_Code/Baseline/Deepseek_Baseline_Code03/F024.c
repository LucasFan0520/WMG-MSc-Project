// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct author {
    char *name;
    struct author *next;
};

struct book {
    char *title;
    int year;
    struct author *authors;
    struct book *next;
};

struct book *books = NULL;

struct book *find_book(char *title) {
    struct book *b = books;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void add_book(char *title, int year) {
    if (find_book(title)) return;
    struct book *b = malloc(sizeof(struct book));
    b->title = malloc(strlen(title) + 1);
    strcpy(b->title, title);
    b->year = year;
    b->authors = NULL;
    b->next = books;
    books = b;
}

void add_author(char *title, char *name) {
    struct book *b = find_book(title);
    if (!b) return;
    struct author *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return;
        a = a->next;
    }
    a = malloc(sizeof(struct author));
    a->name = malloc(strlen(name) + 1);
    strcpy(a->name, name);
    a->next = b->authors;
    b->authors = a;
}

void remove_author(char *title, char *name) {
    struct book *b = find_book(title);
    if (!b) return;
    struct author *a = b->authors, *prev = NULL;
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

void delete_book(char *title) {
    struct book *b = books, *prev = NULL;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else books = b->next;
            free(b->title);
            struct author *a = b->authors;
            while (a) {
                struct author *next = a->next;
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

void find_book_authors(char *title) {
    struct book *b = find_book(title);
    if (b) {
        struct author *a = b->authors;
        while (a) {
            printf("%s\n", a->name);
            a = a->next;
        }
    }
}

void list_books() {
    struct book *b = books;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256];
        int year;
        int n = sscanf(line, "%31s %255s %d %255s", cmd, arg1, &year, arg2);
        if (strcmp(cmd, "BOOK") == 0 && n >= 3) {
            add_book(arg1, year);
        } else if (strcmp(cmd, "AUTHOR") == 0 && n >= 3) {
            // arg2 is name? Actually format: AUTHOR title name
            sscanf(line, "%31s %255s %255s", cmd, arg1, arg2);
            add_author(arg1, arg2);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n >= 3) {
            sscanf(line, "%31s %255s %255s", cmd, arg1, arg2);
            remove_author(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_book(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_book_authors(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_books();
        }
    }
    return 0;
}