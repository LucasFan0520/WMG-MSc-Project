// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *year, **authors; int n_auth; } Book;
Book *books; int n_book;

int find_book(char *title) {
    for(int i=0; i<n_book; i++) if(!strcmp(books[i].title, title)) return i;
    return -1;
}

int main() {
    char cmd[20], title[100], year[100], name[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "BOOK")) {
            scanf("%s %s", title, year);
            books = realloc(books, (n_book+1)*sizeof(Book));
            books[n_book].title = strdup(title);
            books[n_book].year = strdup(year);
            books[n_book].authors = NULL;
            books[n_book].n_auth = 0;
            n_book++;
        } else if(!strcmp(cmd, "AUTHOR")) {
            scanf("%s %s", title, name);
            int i = find_book(title);
            if(i != -1) {
                books[i].authors = realloc(books[i].authors, (books[i].n_auth+1)*sizeof(char*));
                books[i].authors[books[i].n_auth++] = strdup(name);
            }
        } else if(!strcmp(cmd, "REMOVEAUTHOR")) {
            scanf("%s %s", title, name);
            int i = find_book(title);
            if(i != -1) {
                for(int j=0; j<books[i].n_auth; j++) {
                    if(!strcmp(books[i].authors[j], name)) {
                        free(books[i].authors[j]);
                        for(int k=j; k<books[i].n_auth-1; k++) books[i].authors[k] = books[i].authors[k+1];
                        books[i].n_auth--;
                        break;
                    }
                }
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", title);
            int i = find_book(title);
            if(i != -1) {
                free(books[i].title);
                free(books[i].year);
                for(int j=0; j<books[i].n_auth; j++) free(books[i].authors[j]);
                free(books[i].authors);
                for(int k=i; k<n_book-1; k++) books[k] = books[k+1];
                n_book--;
            }
        } else if(!strcmp(cmd, "FIND")) {
            scanf("%s", title);
            int i = find_book(title);
            if(i != -1) {
                for(int j=0; j<books[i].n_auth; j++) {
                    printf("%s\n", books[i].authors[j]);
                }
            }
        } else if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<n_book; i++) {
                printf("%s %s\n", books[i].title, books[i].year);
            }
        }
    }
    return 0;
}