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

Book* find_book(const char *title) {
    Book *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *new = malloc(sizeof(Book));
    new->title = strdup(title);
    new->year = year;
    new->authors = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Book *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void add_author(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Author *cur = b->authors;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return;
        cur = cur->next;
    }
    Author *new = malloc(sizeof(Author));
    new->name = strdup(name);
    new->next = NULL;
    if (!b->authors) b->authors = new;
    else {
        cur = b->authors;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
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
    Book *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->title);
            Author *a = cur->authors;
            while (a) {
                Author *tmp = a;
                a = a->next;
                free(tmp->name);
                free(tmp);
            }
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

void list_books() {
    Book *cur = head;
    while (cur) {
        printf("%s\n", cur->title);
        cur = cur->next;
    }
}

void free_all() {
    Book *cur = head;
    while (cur) {
        Book *tmp = cur;
        cur = cur->next;
        free(tmp->title);
        Author *a = tmp->authors;
        while (a) {
            Author *ta = a;
            a = a->next;
            free(ta->name);
            free(ta);
        }
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *year_str = strtok(NULL, " ");
            if (!year_str) continue;
            int year = atoi(year_str);
            add_book(title, year);
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *name = strtok(NULL, " ");
            if (name) add_author(title, name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *name = strtok(NULL, " ");
            if (name) remove_author(title, name);
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
    free_all();
    return 0;
}