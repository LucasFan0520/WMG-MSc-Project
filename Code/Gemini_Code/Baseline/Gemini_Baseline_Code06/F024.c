// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    int year;
    char **authors;
    int author_count;
} Book;

int main() {
    Book *library = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char title[128];
            int year;
            if (sscanf(line + 5, "%127s %d", title, &year) == 2) {
                library = realloc(library, (count + 1) * sizeof(Book));
                library[count].title = strdup(title);
                library[count].year = year;
                library[count].authors = NULL;
                library[count].author_count = 0;
                count++;
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char title[128], name[128];
            if (sscanf(line + 7, "%127s %127s", title, name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        library[i].authors = realloc(library[i].authors, (library[i].author_count + 1) * sizeof(char *));
                        library[i].authors[library[i].author_count] = strdup(name);
                        library[i].author_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char title[128], name[128];
            if (sscanf(line + 13, "%127s %127s", title, name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(library[i].title, title) == 0) {
                        for (int j = 0; j < library[i].author_count; j++) {
                            if (strcmp(library[i].authors[j], name) == 0) {
                                free(library[i].authors[j]);
                                for (int m = j; m < library[i].author_count - 1; m++) {
                                    library[i].authors[m] = library[i].authors[m + 1];
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(library[i].title, title) == 0) {
                    for (int j = 0; j < library[i].author_count; j++) {
                        printf("%s", library[i].authors[j]);
                        if (j < library[i].author_count - 1) printf(" ");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s (%d)\n", library[i].title, library[i].year);
            }
        }
        free(line);
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
