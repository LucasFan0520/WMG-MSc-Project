// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t a_count;
    size_t a_cap;
} Book;

int main(void) {
    Book *bib = NULL;
    size_t b_count = 0;
    size_t b_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BOOK ", 5) == 0) {
            char *p = line + 5;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *year = "";
            if (*p == ' ') {
                *p = '\0';
                year = p + 1;
            }
            if (b_count >= b_cap) {
                b_cap = b_cap == 0 ? 4 : b_cap * 2;
                Book *nb = realloc(bib, b_cap * sizeof(Book));
                if (!nb) {
                    free(line);
                    break;
                }
                bib = nb;
            }
            bib[b_count].title = safe_dup(title);
            bib[b_count].year = safe_dup(year);
            bib[b_count].authors = NULL;
            bib[b_count].a_count = 0;
            bib[b_count].a_cap = 0;
            b_count++;
        } else if (strncmp(line, "AUTHOR ", 7) == 0) {
            char *p = line + 7;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *name = "";
            if (*p == ' ') {
                *p = '\0';
                name = p + 1;
            }
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    Book *b = &bib[i];
                    if (b->a_count >= b->a_cap) {
                        b->a_cap = b->a_cap == 0 ? 4 : b->a_cap * 2;
                        char **nb = realloc(b->authors, b->a_cap * sizeof(char *));
                        if (!nb) break;
                        b->authors = nb;
                    }
                    b->authors[b->a_count] = safe_dup(name);
                    b->a_count++;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVEAUTHOR ", 13) == 0) {
            char *p = line + 13;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *name = "";
            if (*p == ' ') {
                *p = '\0';
                name = p + 1;
            }
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    Book *b = &bib[i];
                    for (size_t j = 0; j < b->a_count; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            free(b->authors[j]);
                            for (size_t k = j; k < b->a_count - 1; k++) {
                                b->authors[k] = b->authors[k + 1];
                            }
                            b->a_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    free(bib[i].title);
                    free(bib[i].year);
                    for (size_t j = 0; j < bib[i].a_count; j++) {
                        free(bib[i].authors[j]);
                    }
                    free(bib[i].authors);
                    for (size_t j = i; j < b_count - 1; j++) {
                        bib[j] = bib[j + 1];
                    }
                    b_count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    for (size_t j = 0; j < bib[i].a_count; j++) {
                        printf("%s%s", bib[i].authors[j], (j == bib[i].a_count - 1) ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < b_count; i++) {
                printf("%s (%s)\n", bib[i].title, bib[i].year);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < b_count; i++) {
        free(bib[i].title);
        free(bib[i].year);
        for (size_t j = 0; j < bib[i].a_count; j++) {
            free(bib[i].authors[j]);
        }
        free(bib[i].authors);
    }
    free(bib);
    return 0;
}
