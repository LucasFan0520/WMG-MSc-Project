// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* title; int year; char** authors; size_t a_count, a_cap; } Book;

Book* books = NULL;
size_t b_count = 0, b_cap = 0;

void cleanup() {
    for (size_t i = 0; i < b_count; i++) {
        for (size_t j = 0; j < books[i].a_count; j++) free(books[i].authors[j]);
        free(books[i].authors);
        free(books[i].title);
    }
    free(books);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "BOOK") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            int year = atoi(ptr);

            if (b_count == b_cap) {
                size_t new_cap = b_cap == 0 ? 4 : b_cap * 2;
                Book* temp = realloc(books, new_cap * sizeof(Book));
                if (!temp) { cleanup(); free(line); return 1; }
                books = temp; b_cap = new_cap;
            }
            books[b_count].title = strdup(title);
            books[b_count].year = year;
            books[b_count].authors = NULL;
            books[b_count].a_count = 0;
            books[b_count].a_cap = 0;
            if (!books[b_count].title) { cleanup(); free(line); return 1; }
            b_count++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;

            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    if (books[i].a_count == books[i].a_cap) {
                        size_t new_cap = books[i].a_cap == 0 ? 4 : books[i].a_cap * 2;
                        char** temp = realloc(books[i].authors, new_cap * sizeof(char*));
                        if (!temp) { cleanup(); free(line); return 1; }
                        books[i].authors = temp; books[i].a_cap = new_cap;
                    }
                    books[i].authors[books[i].a_count] = strdup(name);
                    if (!books[i].authors[books[i].a_count]) { cleanup(); free(line); return 1; }
                    books[i].a_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;

            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, title) == 0) {
                    for (size_t j = 0; j < books[i].a_count; j++) {
                        if (strcmp(books[i].authors[j], name) == 0) {
                            free(books[i].authors[j]);
                            for (size_t k = j; k < books[i].a_count - 1; k++) books[i].authors[k] = books[i].authors[k + 1];
                            books[i].a_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, ptr) == 0) {
                    for (size_t j = 0; j < books[i].a_count; j++) free(books[i].authors[j]);
                    free(books[i].authors);
                    free(books[i].title);
                    for (size_t j = i; j < b_count - 1; j++) books[j] = books[j + 1];
                    b_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(books[i].title, ptr) == 0) {
                    for (size_t j = 0; j < books[i].a_count; j++) printf("%s\n", books[i].authors[j]);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < b_count; i++) printf("%s %d\n", books[i].title, books[i].year);
        }
    }
    free(line);
    cleanup();
    return 0;
}