// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Auth {
    char *name;
    struct Auth *next;
} Auth;

typedef struct Book {
    char *title;
    int year;
    Auth *auths;
    struct Book *next;
} Book;

Book *books = NULL;

Book *find_book(const char *title) {
    for (Book *b = books; b; b = b->next) {
        if (strcmp(b->title, title) == 0) return b;
    }
    return NULL;
}

void add_book(const char *title, int year) {
    if (find_book(title)) return;
    Book *b = malloc(sizeof(Book));
    b->title = strdup(title);
    b->year = year;
    b->auths = NULL;
    b->next = books;
    books = b;
}

void add_auth(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Auth *a = malloc(sizeof(Auth));
    a->name = strdup(name);
    a->next = b->auths;
    b->auths = a;
}

void remove_auth(const char *title, const char *name) {
    Book *b = find_book(title);
    if (!b) return;
    Auth *prev = NULL, *curr = b->auths;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else b->auths = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_book(const char *title) {
    Book *prev = NULL, *curr = books;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else books = curr->next;
            Auth *a = curr->auths;
            while (a) {
                Auth *next = a->next;
                free(a->name);
                free(a);
                a = next;
            }
            free(curr->title);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_book_auths(const char *title) {
    Book *b = find_book(title);
    if (!b) return;
    for (Auth *a = b->auths; a; a = a->next) {
        printf("%s\n", a->name);
    }
}

void list_books() {
    for (Book *b = books; b; b = b->next) {
        printf("%s %d\n", b->title, b->year);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        int year = 0;
        sscanf(line, "%15s %255s %255s %d", cmd, arg1, arg2, &year);
        
        if (strcmp(cmd, "BOOK") == 0) {
            sscanf(line, "%*s %*s %d", &year);
            add_book(arg1, year);
        }
        else if (strcmp(cmd, "AUTHOR") == 0) add_auth(arg1, arg2);
        else if (strcmp(cmd, "REMOVEAUTHOR") == 0) remove_auth(arg1, arg2);
        else if (strcmp(cmd, "DELETE") == 0) delete_book(arg1);
        else if (strcmp(cmd, "FIND") == 0) find_book_auths(arg1);
        else if (strcmp(cmd, "LIST") == 0) list_books();
        
        free(line);
    }
    return 0;
}