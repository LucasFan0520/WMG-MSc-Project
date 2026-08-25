/* F024.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int auth_count;
    int auth_cap;
} Book;

int main(void) {
    Book *bib = NULL;
    int count = 0;
    int capacity = 0;
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
                char *t = args;
                char *y = space + 1;
                char *n_t = strdup(t);
                char *n_y = strdup(y);
                if (n_t && n_y) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        Book *tmp = realloc(bib, new_cap * sizeof(Book));
                        if (tmp) {
                            bib = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_t); free(n_y);
                            *space = ' ';
                            continue;
                        }
                    }
                    bib[count].title = n_t;
                    bib[count].year = n_y;
                    bib[count].authors = NULL;
                    bib[count].auth_count = 0;
                    bib[count].auth_cap = 0;
                    count++;
                } else {
                    free(n_t); free(n_y);
                }
                *space = ' ';
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *t = args;
                char *name = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(bib[i].title, t) == 0) {
                        char *n_name = strdup(name);
                        if (n_name) {
                            if (bib[i].auth_count >= bib[i].auth_cap) {
                                int new_acap = bib[i].auth_cap == 0 ? 4 : bib[i].auth_cap * 2;
                                char **tmp = realloc(bib[i].authors, new_acap * sizeof(char *));
                                if (tmp) {
                                    bib[i].authors = tmp;
                                    bib[i].auth_cap = new_acap;
                                } else {
                                    free(n_name);
                                    break;
                                }
                            }
                            bib[i].authors[bib[i].auth_count] = n_name;
                            bib[i].auth_count++;
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *args = line + 13;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *t = args;
                char *name = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(bib[i].title, t) == 0) {
                        for (int j = 0; j < bib[i].auth_count; j++) {
                            if (strcmp(bib[i].authors[j], name) == 0) {
                                free(bib[i].authors[j]);
                                for (int k = j; k < bib[i].auth_count - 1; k++) {
                                    bib[i].authors[k] = bib[i].authors[k + 1];
                                }
                                bib[i].auth_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *t = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(bib[i].title, t) == 0) {
                    free(bib[i].title);
                    free(bib[i].year);
                    for (int j = 0; j < bib[i].auth_count; j++) {
                        free(bib[i].authors[j]);
                    }
                    free(bib[i].authors);
                    for (int j = i; j < count - 1; j++) {
                        bib[j] = bib[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *t = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(bib[i].title, t) == 0) {
                    for (int j = 0; j < bib[i].auth_count; j++) {
                        printf("%s\n", bib[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", bib[i].title, bib[i].year);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(bib[i].title);
        free(bib[i].year);
        for (int j = 0; j < bib[i].auth_count; j++) {
            free(bib[i].authors[j]);
        }
        free(bib[i].authors);
    }
    free(bib);
    return 0;
}
