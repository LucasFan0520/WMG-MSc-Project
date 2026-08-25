// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; } Auth;
typedef struct { char *title; int year; Auth *auths; int n, cap; } Book;
Book *books = 0; int n = 0, cap = 0;

int find(char *title) {
    for (int i = 0; i < n; i++) if (!strcmp(books[i].title, title)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256]; int year;
        a[0] = b[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "BOOK")) sscanf(line, "%*s %s %d", a, &year);
        else if (!strcmp(cmd, "AUTHOR") || !strcmp(cmd, "REMOVEAUTHOR")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "BOOK")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; books = realloc(books, cap * sizeof(Book)); }
                books[n].title = strdup(a); books[n].year = year; books[n].auths = 0; books[n].n = 0; books[n].cap = 0; n++;
            }
        } else if (!strcmp(cmd, "AUTHOR")) {
            int i = find(a);
            if (i >= 0) {
                if (books[i].n == books[i].cap) {
                    books[i].cap = books[i].cap ? books[i].cap * 2 : 4;
                    books[i].auths = realloc(books[i].auths, books[i].cap * sizeof(Auth));
                }
                books[i].auths[books[i].n++].name = strdup(b);
            }
        } else if (!strcmp(cmd, "REMOVEAUTHOR")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < books[i].n; j++) {
                    if (!strcmp(books[i].auths[j].name, b)) {
                        free(books[i].auths[j].name);
                        for (int k = j; k < books[i].n - 1; k++) books[i].auths[k] = books[i].auths[k + 1];
                        books[i].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < books[i].n; j++) free(books[i].auths[j].name);
                free(books[i].auths); free(books[i].title);
                for (int j = i; j < n - 1; j++) books[j] = books[j + 1]; n--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < books[i].n; j++) printf("%s ", books[i].auths[j].name);
                if (books[i].n > 0) putchar('\n');
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < n; i++) printf("%s %d\n", books[i].title, books[i].year);
        }
    }
    return 0;
}