// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
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
    size_t auth_count;
    size_t auth_cap;
} Book;

int main(void) {
    Book *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %d\n", arr[i].title, arr[i].year);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                int year = (int)strtol(p2 + 1, NULL, 10);
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Book *narr = realloc(arr, ncap * sizeof(Book));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].title = strdup(title);
                        arr[count].year = year;
                        arr[count].authors = NULL;
                        arr[count].auth_count = 0;
                        arr[count].auth_cap = 0;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        Book *b = &arr[i];
                        if (b->auth_count >= b->auth_cap) {
                            size_t ncap = b->auth_cap == 0 ? 4 : b->auth_cap * 2;
                            char **nauths = realloc(b->authors, ncap * sizeof(char *));
                            if (nauths) {
                                b->authors = nauths;
                                b->auth_cap = ncap;
                            }
                        }
                        if (b->auth_count < b->auth_cap) {
                            b->authors[b->auth_count] = strdup(name);
                            b->auth_count++;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = args;
            char *p2 = strchr(title, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        Book *b = &arr[i];
                        for (size_t j = 0; j < b->auth_count; j++) {
                            if (strcmp(b->authors[j], name) == 0) {
                                free(b->authors[j]);
                                for (size_t k = j; k < b->auth_count - 1; k++) {
                                    b->authors[k] = b->authors[k + 1];
                                }
                                b->auth_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    for (size_t j = 0; j < arr[i].auth_count; j++) {
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
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    for (size_t j = 0; j < arr[i].auth_count; j++) {
                        printf("%s%s", arr[i].authors[j], j == arr[i].auth_count - 1 ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        for (size_t j = 0; j < arr[i].auth_count; j++) {
            free(arr[i].authors[j]);
        }
        free(arr[i].authors);
    }
    free(arr);
    return 0;
}
