// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t author_count;
    size_t author_cap;
} Book;

int main(void) {
    Book *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *year = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Book *nb = realloc(list, cap * sizeof(Book));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].title = strdup(title);
                list[count].year = strdup(year);
                list[count].authors = NULL;
                list[count].author_count = 0;
                list[count].author_cap = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0 || strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    if (strcmp(cmd, "AUTHOR") == 0) {
                        if (list[i].author_count >= list[i].author_cap) {
                            list[i].author_cap = list[i].author_cap == 0 ? 4 : list[i].author_cap * 2;
                            char **na = realloc(list[i].authors, list[i].author_cap * sizeof(char *));
                            if (na) list[i].authors = na;
                        }
                        if (list[i].author_count < list[i].author_cap) {
                            list[i].authors[list[i].author_count++] = strdup(name);
                        }
                    } else {
                        for (size_t j = 0; j < list[i].author_count; j++) {
                            if (strcmp(list[i].authors[j], name) == 0) {
                                free(list[i].authors[j]);
                                for (size_t k = j; k < list[i].author_count - 1; k++) {
                                    list[i].authors[k] = list[i].authors[k + 1];
                                }
                                list[i].author_count--;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    free(list[i].title);
                    free(list[i].year);
                    for (size_t j = 0; j < list[i].author_count; j++) {
                        free(list[i].authors[j]);
                    }
                    free(list[i].authors);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    for (size_t j = 0; j < list[i].author_count; j++) {
                        printf("%s%s", list[i].authors[j], j == list[i].author_count - 1 ? "" : ", ");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s (%s)\n", list[i].title, list[i].year);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].year);
        for (size_t j = 0; j < list[i].author_count; j++) {
            free(list[i].authors[j]);
        }
        free(list[i].authors);
    }
    free(list);
    return 0;
}
