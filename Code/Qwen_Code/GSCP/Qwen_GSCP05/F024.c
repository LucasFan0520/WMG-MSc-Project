// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    int ac;
} Book;

Book *books = NULL;
int bc = 0;

int find_book(const char *title) {
    for (int i = 0; i < bc; i++) {
        if (strcmp(books[i].title, title) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        int yr;
        a1[0] = '\0';
        a2[0] = '\0';
        int n = sscanf(line, "%31s %255s %255s", cmd, a1, a2);
        if (n < 1) continue;

        if (strcmp(cmd, "BOOK") == 0 && n >= 3) {
            yr = atoi(a2);
            if (find_book(a1) < 0) {
                books = realloc(books, (bc + 1) * sizeof(Book));
                if (!books) return 1;
                books[bc].title = strdup(a1);
                books[bc].year = yr;
                books[bc].authors = NULL;
                books[bc].ac = 0;
                bc++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0 && n >= 3) {
            int bi = find_book(a1);
            if (bi >= 0) {
                books[bi].authors = realloc(books[bi].authors, (books[bi].ac + 1) * sizeof(char *));
                if (!books[bi].authors) return 1;
                books[bi].authors[books[bi].ac] = strdup(a2);
                books[bi].ac++;
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && n >= 3) {
            int bi = find_book(a1);
            if (bi >= 0) {
                for (int j = 0; j < books[bi].ac; j++) {
                    if (strcmp(books[bi].authors[j], a2) == 0) {
                        free(books[bi].authors[j]);
                        for (int k = j; k < books[bi].ac - 1; k++) {
                            books[bi].authors[k] = books[bi].authors[k + 1];
                        }
                        books[bi].ac--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int bi = find_book(a1);
            if (bi >= 0) {
                free(books[bi].title);
                for (int j = 0; j < books[bi].ac; j++) {
                    free(books[bi].authors[j]);
                }
                free(books[bi].authors);
                for (int k = bi; k < bc - 1; k++) {
                    books[k] = books[k + 1];
                }
                bc--;
            }
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            int bi = find_book(a1);
            if (bi >= 0) {
                printf("%s %d", books[bi].title, books[bi].year);
                for (int j = 0; j < books[bi].ac; j++) {
                    printf(" %s", books[bi].authors[j]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bc; i++) {
                printf("%s %d\n", books[i].title, books[i].year);
            }
        }
    }
    for (int i = 0; i < bc; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].ac; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
    }
    free(books);
    return 0;
}