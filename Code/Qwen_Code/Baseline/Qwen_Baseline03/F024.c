// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char title[100];
    int year;
    char authors[100][100];
    int acnt;
} Book;

Book books[1000];
int bn = 0;

int findBook(char *title) {
    for (int i = 0; i < bn; i++) {
        if (!strcmp(books[i].title, title)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], title[100], name[100];
    int year;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "BOOK")) {
            scanf("%s %d", title, &year);
            strcpy(books[bn].title, title);
            books[bn].year = year;
            books[bn].acnt = 0;
            bn++;
        } else if (!strcmp(cmd, "AUTHOR")) {
            scanf("%s %s", title, name);
            int i = findBook(title);
            if (i != -1) {
                strcpy(books[i].authors[books[i].acnt++], name);
            }
        } else if (!strcmp(cmd, "REMOVEAUTHOR")) {
            scanf("%s %s", title, name);
            int i = findBook(title);
            if (i != -1) {
                for (int j = 0; j < books[i].acnt; j++) {
                    if (!strcmp(books[i].authors[j], name)) {
                        for (int k = j; k < books[i].acnt - 1; k++) {
                            strcpy(books[i].authors[k], books[i].authors[k + 1]);
                        }
                        books[i].acnt--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", title);
            int i = findBook(title);
            if (i != -1) {
                for (int j = i; j < bn - 1; j++) {
                    books[j] = books[j + 1];
                }
                bn--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", title);
            int i = findBook(title);
            if (i != -1) {
                for (int j = 0; j < books[i].acnt; j++) {
                    printf("%s\n", books[i].authors[j]);
                }
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < bn; i++) {
                printf("%s %d\n", books[i].title, books[i].year);
            }
        }
    }
    return 0;
}