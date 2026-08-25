// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    int year;
    char **authors;
    int author_count;
} Book;

int main() {
    Book *library = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year = 0;
            if (sscanf(args, "%255s %d", title, &year) == 2) {
                library = realloc(library, (count + 1) * sizeof(Book));
                library[count].title = strdup(title);
                library[count].year = year;
                library[count].authors = NULL;
                library[count].author_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(args, "%255s %255s", title, name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        library[i].authors = realloc(library[i].authors, (library[i].author_count + 1) * sizeof(char *));
                        library[i].authors[library[i].author_count] = strdup(name);
                        library[i].author_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], name[256];
            if (sscanf(args, "%255s %255s", title, name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        for (int j = 0; j < library[i].author_count; j++) {
                            if (strcmp(library[i].authors[j], name) == 0) {
                                free(library[i].authors[j]);
                                for (int k = j; k < library[i].author_count - 1; k++) {
                                    library[i].authors[k] = library[i].authors[k + 1];
                                }
                                library[i].author_count--;
                                library[i].authors = realloc(library[i].authors, library[i].author_count * sizeof(char *));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        free(library[i].title);
                        for (int j = 0; j < library[i].author_count; j++) {
                            free(library[i].authors[j]);
                        }
                        free(library[i].authors);
                        for (int j = i; j < count - 1; j++) {
                            library[j] = library[j + 1];
                        }
                        count--;
                        library = realloc(library, count * sizeof(Book));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        for (int j = 0; j < library[i].author_count; j++) {
                            printf("%s%s", library[i].authors[j], (j == library[i].author_count - 1) ? "" : " ");
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s (%d)\n", library[i].title, library[i].year);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(library[i].title);
        for (int j = 0; j < library[i].author_count; j++) {
            free(library[i].authors[j]);
        }
        free(library[i].authors);
    }
    free(library);
    return 0;
}
