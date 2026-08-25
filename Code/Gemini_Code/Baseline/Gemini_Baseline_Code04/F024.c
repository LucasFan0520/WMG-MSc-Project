// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int author_count;
} Book;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Book *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *p = line + 5;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *year = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(Book));
                arr[count].title = strdup(title);
                arr[count].year = strdup(year);
                arr[count].authors = NULL;
                arr[count].author_count = 0;
                count++;
            }
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p = line + 7;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *name = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        int ac = arr[i].author_count;
                        arr[i].authors = realloc(arr[i].authors, (ac + 1) * sizeof(char *));
                        arr[i].authors[ac] = strdup(name);
                        arr[i].author_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p = line + 13;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *name = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        for (int j = 0; j < arr[i].author_count; j++) {
                            if (strcmp(arr[i].authors[j], name) == 0) {
                                free(arr[i].authors[j]);
                                for (int k = j; k < arr[i].author_count - 1; k++) {
                                    arr[i].authors[k] = arr[i].authors[k + 1];
                                }
                                arr[i].author_count--;
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
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].year);
                    for (int j = 0; j < arr[i].author_count; j++) {
                        free(arr[i].authors[j]);
                    }
                    free(arr[i].authors);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    for (int j = 0; j < arr[i].author_count; j++) {
                        printf("%s\n", arr[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].title, arr[i].year);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].year);
        for (int j = 0; j < arr[i].author_count; j++) {
            free(arr[i].authors[j]);
        }
        free(arr[i].authors);
    }
    free(arr);
    return 0;
}
