// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; int year; char **authors; int nauth; } Book;
Book *books = NULL;
int nb = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        int y;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "BOOK") == 0) {
            sscanf(line, "%*s %s %d", a, &y);
            books = realloc(books, (nb + 1) * sizeof(Book));
            books[nb].title = strdup(a);
            books[nb].year = y;
            books[nb].authors = NULL;
            books[nb].nauth = 0;
            nb++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            int idx = -1;
            for (int i = 0; i < nb; i++) if (strcmp(books[i].title, a) == 0) { idx = i; break; }
            if (idx != -1) {
                books[idx].authors = realloc(books[idx].authors, (books[idx].nauth + 1) * sizeof(char*));
                books[idx].authors[books[idx].nauth++] = strdup(t);
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            int idx = -1;
            for (int i = 0; i < nb; i++) if (strcmp(books[i].title, a) == 0) { idx = i; break; }
            if (idx != -1) {
                for (int i = 0; i < books[idx].nauth; i++) {
                    if (strcmp(books[idx].authors[i], t) == 0) {
                        free(books[idx].authors[i]);
                        for (int j = i; j < books[idx].nauth - 1; j++) books[idx].authors[j] = books[idx].authors[j + 1];
                        books[idx].nauth--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nb; i++) {
                if (strcmp(books[i].title, a) == 0) {
                    free(books[i].title);
                    for (int j = 0; j < books[i].nauth; j++) free(books[i].authors[j]);
                    free(books[i].authors);
                    for (int j = i; j < nb - 1; j++) books[j] = books[j + 1];
                    nb--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nb; i++) {
                if (strcmp(books[i].title, a) == 0) {
                    for (int j = 0; j < books[i].nauth; j++) printf("%s\n", books[i].authors[j]);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < nb; i++) printf("%s %d\n", books[i].title, books[i].year);
        }
    }
    for (int i = 0; i < nb; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].nauth; j++) free(books[i].authors[j]);
        free(books[i].authors);
    }
    free(books);
    return 0;
}