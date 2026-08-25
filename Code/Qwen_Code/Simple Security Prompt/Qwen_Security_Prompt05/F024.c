// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    int num_authors;
} Book;

Book *books = NULL;
int num_books = 0;

int find_book(const char *title) {
    for (int i = 0; i < num_books; i++) {
        if (strcmp(books[i].title, title) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year;
            sscanf(line, "%*s %s %d", title, &year);
            books = realloc(books, (num_books + 1) * sizeof(Book));
            books[num_books].title = strdup(title);
            books[num_books].year = year;
            books[num_books].authors = NULL;
            books[num_books].num_authors = 0;
            num_books++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            sscanf(line, "%*s %s %s", title, name);
            int idx = find_book(title);
            if (idx >= 0) {
                books[idx].authors = realloc(books[idx].authors, (books[idx].num_authors + 1) * sizeof(char*));
                books[idx].authors[books[idx].num_authors++] = strdup(name);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            sscanf(line, "%*s %s %s", title, name);
            int idx = find_book(title);
            if (idx >= 0) {
                for (int i = 0; i < books[idx].num_authors; i++) {
                    if (strcmp(books[idx].authors[i], name) == 0) {
                        free(books[idx].authors[i]);
                        for (int j = i; j < books[idx].num_authors - 1; j++) {
                            books[idx].authors[j] = books[idx].authors[j + 1];
                        }
                        books[idx].num_authors--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            sscanf(line, "%*s %s", title);
            int idx = find_book(title);
            if (idx >= 0) {
                for (int i = 0; i < books[idx].num_authors; i++) {
                    free(books[idx].authors[i]);
                }
                free(books[idx].authors);
                free(books[idx].title);
                for (int i = idx; i < num_books - 1; i++) {
                    books[i] = books[i + 1];
                }
                num_books--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            sscanf(line, "%*s %s", title);
            int idx = find_book(title);
            if (idx >= 0) {
                for (int i = 0; i < books[idx].num_authors; i++) {
                    printf("%s\n", books[idx].authors[i]);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < num_books; i++) {
                printf("%s %d\n", books[i].title, books[i].year);
            }
        }
    }
    
    for (int i = 0; i < num_books; i++) {
        for (int j = 0; j < books[i].num_authors; j++) {
            free(books[i].authors[j]);
        }
        free(books[i].authors);
        free(books[i].title);
    }
    free(books);
    
    return 0;
}