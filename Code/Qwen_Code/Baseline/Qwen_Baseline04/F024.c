// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    int acount;
} Book;

Book *books = NULL;
int bcount = 0;

int find_book(const char *title) {
    for (int i = 0; i < bcount; i++) {
        if (strcmp(books[i].title, title) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BOOK") == 0) {
            char title[100];
            int year;
            scanf("%s %d", title, &year);
            if (find_book(title) == -1) {
                books = realloc(books, sizeof(Book) * (bcount + 1));
                books[bcount].title = strdup(title);
                books[bcount].year = year;
                books[bcount].authors = NULL;
                books[bcount].acount = 0;
                bcount++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[100], name[100];
            scanf("%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1) {
                books[idx].authors = realloc(books[idx].authors, sizeof(char*) * (books[idx].acount + 1));
                books[idx].authors[books[idx].acount++] = strdup(name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[100], name[100];
            scanf("%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < books[idx].acount; i++) {
                    if (strcmp(books[idx].authors[i], name) == 0) {
                        free(books[idx].authors[i]);
                        for (int j = i; j < books[idx].acount - 1; j++) {
                            books[idx].authors[j] = books[idx].authors[j+1];
                        }
                        books[idx].acount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[100];
            scanf("%s", title);
            int idx = find_book(title);
            if (idx != -1) {
                free(books[idx].title);
                for (int i = 0; i < books[idx].acount; i++) free(books[idx].authors[i]);
                free(books[idx].authors);
                for (int i = idx; i < bcount - 1; i++) books[i] = books[i+1];
                bcount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[100];
            scanf("%s", title);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < books[idx].acount; i++) {
                    printf("%s\n", books[idx].authors[i]);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bcount; i++) {
                printf("%s %d\n", books[i].title, books[i].year);
            }
        }
    }
    for (int i = 0; i < bcount; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].acount; j++) free(books[i].authors[j]);
        free(books[i].authors);
    }
    free(books);
    return 0;
}