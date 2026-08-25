// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int auth_count;
    int auth_capacity;
} Book;

int main(void) {
    Book *catalog = NULL;
    int count = 0;
    int capacity = 0;
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
            char *year = "";
            char *space = strchr(title, ' ');
            if (space) {
                *space = '\0';
                year = space + 1;
                while (*year == ' ') year++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Book *tmp = realloc(catalog, new_cap * sizeof(Book));
                if (!tmp) return 1;
                catalog = tmp;
                capacity = new_cap;
            }
            catalog[count].title = strdup(title);
            catalog[count].year = strdup(year);
            catalog[count].authors = NULL;
            catalog[count].auth_count = 0;
            catalog[count].auth_capacity = 0;
            count++;
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = space + 1;
            while (*name == ' ') name++;
            int b_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    b_idx = i;
                    break;
                }
            }
            if (b_idx != -1) {
                Book *b = &catalog[b_idx];
                if (b->auth_count >= b->auth_capacity) {
                    int new_acap = b->auth_capacity == 0 ? 4 : b->auth_capacity * 2;
                    char **tmp = realloc(b->authors, new_acap * sizeof(char *));
                    if (!tmp) return 1;
                    b->authors = tmp;
                    b->auth_capacity = new_acap;
                }
                b->authors[b->auth_count] = strdup(name);
                b->auth_count++;
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = space + 1;
            while (*name == ' ') name++;
            int b_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    b_idx = i;
                    break;
                }
            }
            if (b_idx != -1) {
                Book *b = &catalog[b_idx];
                int a_idx = -1;
                for (int j = 0; j < b->auth_count; j++) {
                    if (strcmp(b->authors[j], name) == 0) {
                        a_idx = j;
                        break;
                    }
                }
                if (a_idx != -1) {
                    free(b->authors[a_idx]);
                    for (int j = a_idx; j < b->auth_count - 1; j++) {
                        b->authors[j] = b->authors[j + 1];
                    }
                    b->auth_count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            int b_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    b_idx = i;
                    break;
                }
            }
            if (b_idx != -1) {
                free(catalog[b_idx].title);
                free(catalog[b_idx].year);
                for (int j = 0; j < catalog[b_idx].auth_count; j++) {
                    free(catalog[b_idx].authors[j]);
                }
                free(catalog[b_idx].authors);
                for (int i = b_idx; i < count - 1; i++) {
                    catalog[i] = catalog[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            int b_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    b_idx = i;
                    break;
                }
            }
            if (b_idx != -1) {
                printf("Book: %s (%s) Authors:", catalog[b_idx].title, catalog[b_idx].year);
                for (int j = 0; j < catalog[b_idx].auth_count; j++) {
                    printf(" %s", catalog[b_idx].authors[j]);
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        free(catalog[i].year);
        for (int j = 0; j < catalog[i].auth_count; j++) {
            free(catalog[i].authors[j]);
        }
        free(catalog[i].authors);
    }
    free(catalog);
    return 0;
}
