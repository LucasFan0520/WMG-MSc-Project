/* F024.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
} Author;

typedef struct {
    char *title;
    int year;
    Author *authors;
    int acount;
    int acap;
} Book;

static Book *books = NULL;
static int bcount = 0;
static int bcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_book(const char *title) {
    for (int i = 0; i < bcount; i++)
        if (strcmp(books[i].title, title) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "BOOK") == 0) {
            char title[256] = {0};
            int year = 0;
            if (sscanf(line + offset, "%255s %d", title, &year) != 2) continue;
            if (find_book(title) >= 0) continue;
            if (bcount >= bcap) {
                bcap = bcap == 0 ? 8 : bcap * 2;
                books = realloc(books, sizeof(Book) * bcap);
            }
            books[bcount].title = dup_str(title);
            books[bcount].year = year;
            books[bcount].authors = NULL;
            books[bcount].acount = 0;
            books[bcount].acap = 0;
            bcount++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256] = {0}, name[256] = {0};
            if (sscanf(line + offset, "%255s %255s", title, name) != 2) continue;
            int idx = find_book(title);
            if (idx < 0) continue;
            Book *b = &books[idx];
            if (b->acount >= b->acap) {
                b->acap = b->acap == 0 ? 4 : b->acap * 2;
                b->authors = realloc(b->authors, sizeof(Author) * b->acap);
            }
            b->authors[b->acount++].name = dup_str(name);
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256] = {0}, name[256] = {0};
            if (sscanf(line + offset, "%255s %255s", title, name) != 2) continue;
            int idx = find_book(title);
            if (idx < 0) continue;
            Book *b = &books[idx];
            for (int i = 0; i < b->acount; i++) {
                if (strcmp(b->authors[i].name, name) == 0) {
                    free(b->authors[i].name);
                    for (int j = i; j < b->acount - 1; j++) b->authors[j] = b->authors[j + 1];
                    b->acount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, "%255s", title) != 1) continue;
            int idx = find_book(title);
            if (idx < 0) continue;
            free(books[idx].title);
            for (int i = 0; i < books[idx].acount; i++) free(books[idx].authors[i].name);
            free(books[idx].authors);
            for (int i = idx; i < bcount - 1; i++) books[i] = books[i + 1];
            bcount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, "%255s", title) != 1) continue;
            int idx = find_book(title);
            if (idx < 0) { printf("NOT_FOUND\n"); continue; }
            for (int i = 0; i < books[idx].acount; i++) printf("%s\n", books[idx].authors[i].name);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bcount; i++) printf("%s %d\n", books[i].title, books[i].year);
        }
    }
    for (int i = 0; i < bcount; i++) {
        free(books[i].title);
        for (int j = 0; j < books[i].acount; j++) free(books[i].authors[j].name);
        free(books[i].authors);
    }
    free(books);
    return 0;
}