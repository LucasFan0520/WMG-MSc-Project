// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char *authors[100];
    int ac;
} Book;

Book books[1000];
int bc = 0;

int find_book(char *title) {
    for (int i = 0; i < bc; i++) if (strcmp(books[i].title, title) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "BOOK") == 0) {
            char title[1000];
            int year;
            sscanf(rest, "%s %d", title, &year);
            if (find_book(title) == -1) {
                books[bc].title = strdup(title);
                books[bc].year = year;
                books[bc].ac = 0;
                bc++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[1000], name[1000];
            sscanf(rest, "%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1) {
                books[idx].authors[books[idx].ac++] = strdup(name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[1000], name[1000];
            sscanf(rest, "%s %s", title, name);
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < books[idx].ac; i++) {
                    if (strcmp(books[idx].authors[i], name) == 0) {
                        free(books[idx].authors[i]);
                        for (int j = i; j < books[idx].ac - 1; j++) books[idx].authors[j] = books[idx].authors[j+1];
                        books[idx].ac--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_book(rest);
            if (idx != -1) {
                free(books[idx].title);
                for (int i = 0; i < books[idx].ac; i++) free(books[idx].authors[i]);
                for (int i = idx; i < bc - 1; i++) books[i] = books[i+1];
                bc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            int idx = find_book(rest);
            if (idx != -1) {
                for (int i = 0; i < books[idx].ac; i++) {
                    printf("%s\n", books[idx].authors[i]);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bc; i++) {
                printf("%s %d\n", books[i].title, books[i].year);
            }
        }
    }
    for (int i = 0; i < bc; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].ac; j++) free(books[i].authors[j]);
    }
    return 0;
}