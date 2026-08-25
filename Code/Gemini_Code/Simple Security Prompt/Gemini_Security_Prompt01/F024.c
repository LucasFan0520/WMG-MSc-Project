// F024.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int author_count;
} Book;

int main() {
    Book *library = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "BOOK") == 0 && args) {
            char *title = args;
            char *space2 = strchr(args, ' ');
            char *year = "";
            if (space2) {
                *space2 = '\0';
                year = space2 + 1;
                while (*year == ' ') year++;
            }
            library = realloc(library, (count + 1) * sizeof(Book));
            if (!library) return 1;
            library[count].title = strdup(title);
            library[count].year = strdup(year);
            library[count].authors = NULL;
            library[count].author_count = 0;
            count++;
        } else if (strcmp(cmd, "AUTHOR") == 0 && args) {
            char *title = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *auth = space2 + 1;
                while (*auth == ' ') auth++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        int ac = library[i].author_count;
                        library[i].authors = realloc(library[i].authors, (ac + 1) * sizeof(char *));
                        if (!library[i].authors) return 1;
                        library[i].authors[ac] = strdup(auth);
                        library[i].author_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0 && args) {
            char *title = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *auth = space2 + 1;
                while (*auth == ' ') auth++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        for (int j = 0; j < library[i].author_count; j++) {
                            if (strcmp(library[i].authors[j], auth) == 0) {
                                free(library[i].authors[j]);
                                for (int k = j; k < library[i].author_count - 1; k++) {
                                    library[i].authors[k] = library[i].authors[k + 1];
                                }
                                library[i].author_count--;
                                library[i].authors = realloc(library[i].authors, library[i].author_count * sizeof(char *));
                                if (library[i].author_count > 0 && !library[i].authors) return 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(library[i].title, args) == 0) {
                    free(library[i].title);
                    free(library[i].year);
                    for (int j = 0; j < library[i].author_count; j++) {
                        free(library[i].authors[j]);
                    }
                    free(library[i].authors);
                    for (int j = i; j < count - 1; j++) {
                        library[j] = library[j + 1];
                    }
                    count--;
                    library = realloc(library, count * sizeof(Book));
                    if (count > 0 && !library) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(library[i].title, args) == 0) {
                    for (int j = 0; j < library[i].author_count; j++) {
                        printf("%s%s", library[i].authors[j], (j == library[i].author_count - 1) ? "" : " ");
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", library[i].title, library[i].year);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(library[i].title);
        free(library[i].year);
        for (int j = 0; j < library[i].author_count; j++) {
            free(library[i].authors[j]);
        }
        free(library[i].authors);
    }
    free(library);
    return 0;
}
