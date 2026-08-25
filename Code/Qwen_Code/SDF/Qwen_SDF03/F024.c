// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

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

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Book *find_book(const char *title) {
    for (Book *b = head; b; b = b->next)
        if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

void free_book(Book *b) {
    Author *a = b->authors;
    while (a) {
        Author *next = a->next;
        free(a->name);
        free(a);
        a = next;
    }
    free(b->title);
    free(b);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = get_token(&ptr);
            char *yr = get_token(&ptr);
            if (title && yr && !find_book(title)) {
                Book *b = malloc(sizeof(Book));
                if (b) {
                    b->title = strdup(title);
                    b->year = atoi(yr);
                    b->authors = NULL;
                    b->next = head;
                    head = b;
                }
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = get_token(&ptr);
            char *name = get_token(&ptr);
            if (title && name) {
                Book *b = find_book(title);
                if (b) {
                    Author *a = malloc(sizeof(Author));
                    if (a) {
                        a->name = strdup(name);
                        a->next = b->authors;
                        b->authors = a;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = get_token(&ptr);
            char *name = get_token(&ptr);
            if (title && name) {
                Book *b = find_book(title);
                if (b) {
                    Author *prev = NULL, *curr = b->authors;
                    while (curr) {
                        if (strcmp(curr->name, name) == 0) {
                            if (prev) prev->next = curr->next;
                            else b->authors = curr->next;
                            free(curr->name);
                            free(curr);
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Book *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->title, title) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free_book(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Book *b = find_book(title);
                if (b) {
                    for (Author *a = b->authors; a; a = a->next) {
                        printf("%s\n", a->name);
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Book *b = head; b; b = b->next) {
                printf("%s %d\n", b->title, b->year);
            }
        }
    }
    while (head) {
        Book *next = head->next;
        free_book(head);
        head = next;
    }
    return 0;
}