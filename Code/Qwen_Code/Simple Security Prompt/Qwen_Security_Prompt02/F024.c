// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
} Author;

typedef struct {
    char *title;
    int year;
    Author *authors;
    int a_count;
    int a_cap;
} Book;

Book *books = NULL;
int b_count = 0;

int find_book(const char *title) {
    for (int i = 0; i < b_count; i++) {
        if (strcmp(books[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int find_author(int bi, const char *name) {
    for (int i = 0; i < books[bi].a_count; i++) {
        if (strcmp(books[bi].authors[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char title[1024];
    char name[1024];
    int year;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BOOK") == 0) {
            scanf("%s %d", title, &year);
            if (find_book(title) == -1) {
                books = realloc(books, sizeof(Book) * (b_count + 1));
                books[b_count].title = strdup(title);
                books[b_count].year = year;
                books[b_count].authors = NULL;
                books[b_count].a_count = 0;
                books[b_count].a_cap = 0;
                b_count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            scanf("%s %s", title, name);
            int bi = find_book(title);
            if (bi != -1) {
                if (books[bi].a_count == books[bi].a_cap) {
                    books[bi].a_cap = books[bi].a_cap ? books[bi].a_cap * 2 : 4;
                    books[bi].authors = realloc(books[bi].authors, sizeof(Author) * books[bi].a_cap);
                }
                books[bi].authors[books[bi].a_count].name = strdup(name);
                books[bi].a_count++;
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            scanf("%s %s", title, name);
            int bi = find_book(title);
            if (bi != -1) {
                int ai = find_author(bi, name);
                if (ai != -1) {
                    free(books[bi].authors[ai].name);
                    for (int i = ai; i < books[bi].a_count - 1; i++) {
                        books[bi].authors[i] = books[bi].authors[i + 1];
                    }
                    books[bi].a_count--;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", title);
            int bi = find_book(title);
            if (bi != -1) {
                free(books[bi].title);
                for (int i = 0; i < books[bi].a_count; i++) {
                    free(books[bi].authors[i].name);
                }
                free(books[bi].authors);
                for (int i = bi; i < b_count - 1; i++) {
                    books[i] = books[i + 1];
                }
                b_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int bi = find_book(title);
            if (bi != -1) {
                printf("%s %d", books[bi].title, books[bi].year);
                for (int i = 0; i < books[bi].a_count; i++) {
                    printf(" %s", books[bi].authors[i].name);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < b_count; i++) {
                printf("%s %d\n", books[i].title, books[i].year);
            }
        }
    }

    for (int i = 0; i < b_count; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].a_count; j++) {
            free(books[i].authors[j].name);
        }
        free(books[i].authors);
    }
    free(books);

    return 0;
}