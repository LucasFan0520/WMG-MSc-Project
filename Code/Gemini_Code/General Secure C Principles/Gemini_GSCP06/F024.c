// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t auth_count;
    size_t auth_capacity;
} Book;

int main(void) {
    Book *catalog = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *title = args;
                char *year = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Book *next = realloc(catalog, capacity * sizeof(Book));
                    if (!next) break;
                    catalog = next;
                }
                catalog[count].title = strdup(title);
                catalog[count].year = strdup(year);
                catalog[count].authors = NULL;
                catalog[count].auth_count = 0;
                catalog[count].auth_capacity = 0;
                count++;
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *title = args;
                char *name = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        if (catalog[i].auth_count >= catalog[i].auth_capacity) {
                            catalog[i].auth_capacity = catalog[i].auth_capacity == 0 ? 4 : catalog[i].auth_capacity * 2;
                            char **nxt = realloc(catalog[i].authors, catalog[i].auth_capacity * sizeof(char *));
                            if (!nxt) break;
                            catalog[i].authors = nxt;
                        }
                        catalog[i].authors[catalog[i].auth_count++] = strdup(name);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *args = line + 13;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *title = args;
                char *name = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(catalog[i].title, title) == 0) {
                        for (size_t k = 0; k < catalog[i].auth_count; k++) {
                            if (strcmp(catalog[i].authors[k], name) == 0) {
                                free(catalog[i].authors[k]);
                                for (size_t j = k; j < catalog[i].auth_count - 1; j++) {
                                    catalog[i].authors[j] = catalog[i].authors[j + 1];
                                }
                                catalog[i].auth_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    free(catalog[i].year);
                    for (size_t k = 0; k < catalog[i].auth_count; k++) {
                        free(catalog[i].authors[k]);
                    }
                    free(catalog[i].authors);
                    for (size_t j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    for (size_t k = 0; k < catalog[i].auth_count; k++) {
                        printf("%s%s", catalog[i].authors[k], (k == catalog[i].auth_count - 1) ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", catalog[i].title, catalog[i].year);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(catalog[i].title);
        free(catalog[i].year);
        for (size_t k = 0; k < catalog[i].auth_count; k++) {
            free(catalog[i].authors[k]);
        }
        free(catalog[i].authors);
    }
    free(catalog);
    free(line);
    return 0;
}
