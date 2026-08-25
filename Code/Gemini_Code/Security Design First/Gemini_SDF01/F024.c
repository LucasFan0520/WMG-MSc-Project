// F024.c
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
    Book *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "BOOK") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *year = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Book *new_items = realloc(items, capacity * sizeof(Book));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].title = strdup(title);
                items[count].year = strdup(year);
                if (!items[count].title || !items[count].year) exit(1);
                items[count].authors = NULL;
                items[count].author_count = 0;
                items[count].author_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    Book *b = &items[i];
                    if (b->author_count >= b->author_capacity) {
                        b->author_capacity = b->author_capacity == 0 ? 4 : b->author_capacity * 2;
                        char **new_auths = realloc(b->authors, b->author_capacity * sizeof(char *));
                        if (!new_auths) exit(1);
                        b->authors = new_auths;
                    }
                    b->authors[b->author_count] = strdup(name);
                    if (!b->authors[b->author_count]) exit(1);
                    b->author_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    Book *b = &items[i];
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
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    free(items[i].year);
                    for (size_t j = 0; j < items[i].author_count; j++) {
                        free(items[i].authors[j]);
                    }
                    free(items[i].authors);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    for (size_t j = 0; j < items[i].author_count; j++) {
                        printf("%s%s", items[i].authors[j], (j == items[i].author_count - 1) ? "" : " ");
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s (%s)\n", items[i].title, items[i].year);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].year);
        for (size_t j = 0; j < items[i].author_count; j++) {
            free(items[i].authors[j]);
        }
        free(items[i].authors);
    }
    free(items);
    free(line);
    return 0;
}
