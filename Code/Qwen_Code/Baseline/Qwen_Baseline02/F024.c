// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; int year; char **authors; int nauthors; } Book;
Book books[1000]; int nbooks = 0;

int find_book(char *title) {
    for (int i = 0; i < nbooks; i++) if (!strcmp(books[i].title, title)) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], name[100]; int year;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "BOOK")) {
            scanf("%s %d", title, &year);
            books[nbooks].title = strdup(title);
            books[nbooks].year = year;
            books[nbooks].authors = NULL;
            books[nbooks++].nauthors = 0;
        } else if (!strcmp(cmd, "AUTHOR")) {
            scanf("%s", title);
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            if (name[0] == ' ') memmove(name, name + 1, strlen(name));
            int idx = find_book(title);
            if (idx != -1) {
                books[idx].authors = realloc(books[idx].authors, sizeof(char*) * (books[idx].nauthors + 1));
                books[idx].authors[books[idx].nauthors++] = strdup(name);
            }
        } else if (!strcmp(cmd, "REMOVEAUTHOR")) {
            scanf("%s", title);
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            if (name[0] == ' ') memmove(name, name + 1, strlen(name));
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < books[idx].nauthors; i++) {
                    if (!strcmp(books[idx].authors[i], name)) {
                        free(books[idx].authors[i]);
                        for (int j = i; j < books[idx].nauthors - 1; j++) books[idx].authors[j] = books[idx].authors[j + 1];
                        books[idx].nauthors--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", title);
            int idx = find_book(title);
            if (idx != -1) {
                free(books[idx].title);
                for (int i = 0; i < books[idx].nauthors; i++) free(books[idx].authors[i]);
                free(books[idx].authors);
                for (int i = idx; i < nbooks - 1; i++) books[i] = books[i + 1];
                nbooks--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", title);
            int idx = find_book(title);
            if (idx != -1) {
                printf("%d", books[idx].year);
                for (int i = 0; i < books[idx].nauthors; i++) printf(" %s", books[idx].authors[i]);
                printf("\n");
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < nbooks; i++) printf("%s %d\n", books[i].title, books[i].year);
        }
    }
    for (int i = 0; i < nbooks; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].nauthors; j++) free(books[i].authors[j]);
        free(books[i].authors);
    }
    return 0;
}