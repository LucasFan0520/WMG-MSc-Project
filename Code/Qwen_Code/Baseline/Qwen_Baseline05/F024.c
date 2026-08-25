// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 100
#define MAX_NAME 100

typedef struct {
    char title[MAX_TITLE];
    int year;
    char authors[100][MAX_NAME];
    int auth_count;
} Book;

Book books[1000];
int book_count = 0;

int find_book(const char *title) {
    for (int i = 0; i < book_count; i++) if (strcmp(books[i].title, title) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], title[MAX_TITLE], name[MAX_NAME];
    int year;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BOOK") == 0) {
            scanf("%s %d", title, &year);
            strcpy(books[book_count].title, title);
            books[book_count].year = year;
            books[book_count].auth_count = 0;
            book_count++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            scanf("%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1) {
                strcpy(books[idx].authors[books[idx].auth_count], name);
                books[idx].auth_count++;
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            scanf("%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < books[idx].auth_count; i++) {
                    if (strcmp(books[idx].authors[i], name) == 0) {
                        for (int j = i; j < books[idx].auth_count - 1; j++) {
                            strcpy(books[idx].authors[j], books[idx].authors[j + 1]);
                        }
                        books[idx].auth_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", title);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = idx; i < book_count - 1; i++) books[i] = books[i + 1];
                book_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < books[idx].auth_count; i++) printf("%s\n", books[idx].authors[i]);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < book_count; i++) printf("%s %d\n", books[i].title, books[i].year);
        }
    }
    return 0;
}