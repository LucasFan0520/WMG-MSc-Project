// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int a_count;
    int a_capacity;
} Book;

Book *library = NULL;
int b_count = 0;
int b_capacity = 0;

int find_book(const char *title) {
    for (int i = 0; i < b_count; i++) {
        if (strcmp(library[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *p1 = line + 5;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *year = sp + 1;
                if (find_book(p1) == -1) {
                    if (b_count >= b_capacity) {
                        b_capacity = b_capacity == 0 ? 4 : b_capacity * 2;
                        library = realloc(library, b_capacity * sizeof(Book));
                    }
                    library[b_count].title = strdup(p1);
                    library[b_count].year = strdup(year);
                    library[b_count].authors = NULL;
                    library[b_count].a_count = 0;
                    library[b_count].a_capacity = 0;
                    b_count++;
                }
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p1 = line + 7;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *name = sp + 1;
                int idx = find_book(p1);
                if (idx != -1) {
                    Book *b = &library[idx];
                    if (b->a_count >= b->a_capacity) {
                        b->a_capacity = b->a_capacity == 0 ? 4 : b->a_capacity * 2;
                        b->authors = realloc(b->authors, b->a_capacity * sizeof(char *));
                    }
                    b->authors[b->a_count] = strdup(name);
                    b->a_count++;
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p1 = line + 13;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *name = sp + 1;
                int idx = find_book(p1);
                if (idx != -1) {
                    Book *b = &library[idx];
                    for (int i = 0; i < b->a_count; i++) {
                        if (strcmp(b->authors[i], name) == 0) {
                            free(b->authors[i]);
                            for (int j = i; j < b->a_count - 1; j++) {
                                b->authors[j] = b->authors[j + 1];
                            }
                            b->a_count--;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            int idx = find_book(title);
            if (idx != -1) {
                free(library[idx].title);
                free(library[idx].year);
                for (int i = 0; i < library[idx].a_count; i++) {
                    free(library[idx].authors[i]);
                }
                free(library[idx].authors);
                for (int i = idx; i < b_count - 1; i++) {
                    library[i] = library[i + 1];
                }
                b_count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            int idx = find_book(title);
            if (idx != -1) {
                for (int i = 0; i < library[idx].a_count; i++) {
                    printf("%s\n", library[idx].authors[i]);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < b_count; i++) {
                printf("%s %s\n", library[i].title, library[i].year);
            }
        }
    }
    for (int i = 0; i < b_count; i++) {
        free(library[i].title);
        free(library[i].year);
        for (int j = 0; j < library[i].a_count; j++) {
            free(library[i].authors[j]);
        }
        free(library[i].authors);
    }
    free(library);
    return 0;
}
