// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t author_count;
    size_t author_capacity;
} Book;

int main(void) {
    Book *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *year = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                year = p;
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Book *tmp = realloc(arr, capacity * sizeof(Book));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].title = strdup(title);
                arr[count].year = strdup(year);
                arr[count].authors = NULL;
                arr[count].author_count = 0;
                arr[count].author_capacity = 0;
                if (!arr[count].title || !arr[count].year) exit(1);
                count++;
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *name = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                name = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    Book *b = &arr[i];
                    if (b->author_count >= b->author_capacity) {
                        b->author_capacity = b->author_capacity == 0 ? 4 : b->author_capacity * 2;
                        char **tmp = realloc(b->authors, b->author_capacity * sizeof(char *));
                        if (!tmp) exit(1);
                        b->authors = tmp;
                    }
                    b->authors[b->author_count] = strdup(name);
                    if (!b->authors[b->author_count]) exit(1);
                    b->author_count++;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *name = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                name = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    Book *b = &arr[i];
                    for (size_t j = 0; j < b->author_count; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            free(b->authors[j]);
                            for (size_t k = j; k < b->author_count - 1; k++) {
                                b->authors[k] = b->authors[k + 1];
                            }
                            b->author_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].year);
                    for (size_t j = 0; j < arr[i].author_count; j++) {
                        free(arr[i].authors[j]);
                    }
                    free(arr[i].authors);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    for (size_t j = 0; j < arr[i].author_count; j++) {
                        printf("%s\n", arr[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s (%s)\n", arr[i].title, arr[i].year);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].year);
        for (size_t j = 0; j < arr[i].author_count; j++) {
            free(arr[i].authors[j]);
        }
        free(arr[i].authors);
    }
    free(arr);
    free(line);
    return 0;
}
