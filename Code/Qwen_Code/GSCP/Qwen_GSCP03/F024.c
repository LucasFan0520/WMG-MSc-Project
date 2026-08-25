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

void free_authors(Author *a) {
    while (a) { Author *tmp = a; a = a->next; free(tmp->name); free(tmp); }
}

void free_list() {
    while (head) {
        Book *b = head; head = head->next;
        free(b->title); free_authors(b->authors); free(b);
    }
}

Book *find_book(const char *title) {
    for (Book *b = head; b; b = b->next)
        if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], title[256], name[256];
        int year;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "BOOK") == 0) {
            if (sscanf(line, "%*s %255s %d", title, &year) != 2) continue;
            if (find_book(title)) continue;
            Book *b = malloc(sizeof(Book));
            b->title = strdup(title); b->year = year; b->authors = NULL;
            b->next = head; head = b;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", title, name) != 2) continue;
            Book *b = find_book(title);
            if (b) {
                Author *a = malloc(sizeof(Author));
                a->name = strdup(name); a->next = b->authors; b->authors = a;
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            if (sscanf(line, "%*s %255s %255s", title, name) != 2) continue;
            Book *b = find_book(title);
            if (b) {
                Author *prev = NULL, *curr = b->authors;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        Author *tmp = curr; curr = curr->next;
                        if (prev) prev->next = curr; else b->authors = curr;
                        free(tmp->name); free(tmp);
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Book *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    Book *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->title); free_authors(tmp->authors); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) != 1) continue;
            Book *b = find_book(title);
            if (b) {
                printf("%s %d", b->title, b->year);
                for (Author *a = b->authors; a; a = a->next) printf(" %s", a->name);
                printf("\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Book *b = head; b; b = b->next) printf("%s %d\n", b->title, b->year);
        }
    }
    free_list();
    return 0;
}