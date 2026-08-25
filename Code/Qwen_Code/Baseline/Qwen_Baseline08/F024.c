// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 1000
#define MAX_AUTH 100

typedef struct {
    char title[100];
    int year;
    char authors[MAX_AUTH][100];
    int acount;
} Book;

Book books[MAX_BOOKS];
int bcount = 0;

int find_book(const char *title) {
    for (int i = 0; i < bcount; i++) if (strcmp(books[i].title, title) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], name[100];
    int year;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BOOK") == 0) {
            scanf("%s %d", title, &year);
            if (find_book(title) == -1 && bcount < MAX_BOOKS) {
                strcpy(books[bcount].title, title);
                books[bcount].year = year;
                books[bcount].acount = 0;
                bcount++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            scanf("%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1 && books[idx].acount < MAX_AUTH) {
                strcpy(books[idx].authors[books[idx].acount++], name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            scanf("%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < books[idx].acount; i++) {
                    if (strcmp(books[idx].authors[i], name) == 0) {
                        for (int j = i; j < books[idx].acount - 1; j++) {
                            strcpy(books[idx].authors[j], books[idx].authors[j + 1]);
                        }
                        books[idx].acount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", title);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = idx; i < bcount - 1; i++) books[i] = books[i + 1];
                bcount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
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
    return 0;
}