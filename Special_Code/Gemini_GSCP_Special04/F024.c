/* F024.c */
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
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
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *year = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (b_count >= b_cap) {
                    b_cap = b_cap == 0 ? 4 : b_cap * 2;
                    Book *nb = realloc(bib, b_cap * sizeof(Book));
                    if (nb) bib = nb;
                }
                char *nt = mystrdup(title);
                char *ny = mystrdup(year);
                if (nt && ny) {
                    bib[b_count].title = nt;
                    bib[b_count].year = ny;
                    bib[b_count].authors = NULL;
                    bib[b_count].a_count = 0;
                    bib[b_count].a_cap = 0;
                    b_count++;
                } else {
                    free(nt); free(ny);
                }
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Book *b = &bib[found];
                if (b->a_count >= b->a_cap) {
                    b->a_cap = b->a_cap == 0 ? 4 : b->a_cap * 2;
                    char **na = realloc(b->authors, b->a_cap * sizeof(char *));
                    if (na) b->authors = na;
                }
                char *nn = mystrdup(name);
                if (nn) {
                    b->authors[b->a_count++] = nn;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    Book *b = &bib[i];
                    int aidx = -1;
                    for (size_t j = 0; j < b->a_count; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            aidx = (int)j;
                            break;
                        }
                    }
                    if (aidx != -1) {
                        free(b->authors[aidx]);
                        for (size_t j = (size_t)aidx; j < b->a_count - 1; j++) {
                            b->authors[j] = b->authors[j + 1];
                        }
                        b->a_count--;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(bib[found].title);
                free(bib[found].year);
                for (size_t j = 0; j < bib[found].a_count; j++) {
                    free(bib[found].authors[j]);
                }
                free(bib[found].authors);
                for (size_t i = (size_t)found; i < b_count - 1; i++) {
                    bib[i] = bib[i + 1];
                }
                b_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < b_count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    for (size_t j = 0; j < bib[i].a_count; j++) {
                        printf("%s", bib[i].authors[j]);
                        if (j + 1 < bib[i].a_count) putchar(' ');
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
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
