// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AuthorNode {
    char *name;
    struct AuthorNode *next;
} AuthorNode;

typedef struct Book {
    char *title;
    int year;
    AuthorNode *authors;
    struct Book *next;
} Book;

Book *head = NULL;

Book* find_book(const char *title) {
    Book *b = head;
    while (b) {
        if (strcmp(b->title, title) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void free_authors(AuthorNode *a) {
    while (a) {
        AuthorNode *tmp = a;
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

void handle_book(char *cmd) {
    char *title = strtok(NULL, " ");
    if (!title) return;
    char *yearstr = strtok(NULL, " ");
    if (!yearstr) return;
    if (find_book(title)) return;
    Book *new = malloc(sizeof(Book));
    new->title = strdup(title);
    new->year = atoi(yearstr);
    new->authors = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Book *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_author(char *cmd) {
    char *title = strtok(NULL, " ");
    if (!title) return;
    Book *b = find_book(title);
    if (!b) return;
    char *name = strtok(NULL, " ");
    if (!name) return;
    AuthorNode *a = b->authors;
    while (a) {
        if (strcmp(a->name, name) == 0) return;
        a = a->next;
    }
    AuthorNode *new = malloc(sizeof(AuthorNode));
    new->name = strdup(name);
    new->next = NULL;
    if (!b->authors) b->authors = new;
    else {
        AuthorNode *last = b->authors;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_removeauthor(char *cmd) {
    char *title = strtok(NULL, " ");
    if (!title) return;
    Book *b = find_book(title);
    if (!b) return;
    char *name = strtok(NULL, " ");
    if (!name) return;
    AuthorNode *a = b->authors, *prev = NULL;
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

void handle_delete(char *cmd) {
    char *title = strtok(NULL, " ");
    if (!title) return;
    Book *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->title, title) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free_book(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void handle_find(char *cmd) {
    char *title = strtok(NULL, " ");
    if (!title) return;
    Book *b = find_book(title);
    if (!b) return;
    AuthorNode *a = b->authors;
    while (a) {
        printf("%s\n", a->name);
        a = a->next;
    }
}

void handle_list() {
    Book *b = head;
    while (b) {
        printf("%s %d\n", b->title, b->year);
        b = b->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BOOK") == 0) handle_book(cmd);
        else if (strcmp(cmd, "AUTHOR") == 0) handle_author(cmd);
        else if (strcmp(cmd, "REMOVEAUTHOR") == 0) handle_removeauthor(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "FIND") == 0) handle_find(cmd);
        else if (strcmp(cmd, "LIST") == 0) handle_list();
    }
    while (head) {
        Book *tmp = head;
        head = head->next;
        free_book(tmp);
    }
    return 0;
}