// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    size_t acount;
    size_t acap;
} Book;

int main(void) {
    Book *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *year = p;
            size_t ylen = strlen(year);
            while (ylen > 0 && (year[ylen-1] == '\n' || year[ylen-1] == '\n')) {
                year[ylen-1] = '\0';
                ylen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Book *nitems = realloc(items, cap * sizeof(Book));
                if (nitems) items = nitems;
            }
            items[count].title = mystrdup(title);
            items[count].year = mystrdup(year);
            items[count].authors = NULL;
            items[count].acount = 0;
            items[count].acap = 0;
            count++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    Book *b = &items[i];
                    if (b->acount >= b->acap) {
                        b->acap = b->acap == 0 ? 4 : b->acap * 2;
                        char **na = realloc(b->authors, b->acap * sizeof(char*));
                        if (na) b->authors = na;
                    }
                    b->authors[b->acount] = mystrdup(name);
                    b->acount++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    Book *b = &items[i];
                    for (size_t j = 0; j < b->acount; j++) {
                        if (strcmp(b->authors[j], name) == 0) {
                            free(b->authors[j]);
                            for (size_t k = j; k < b->acount - 1; k++) {
                                b->authors[k] = b->authors[k+1];
                            }
                            b->acount--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    free(items[i].year);
                    for (size_t j = 0; j < items[i].acount; j++) {
                        free(items[i].authors[j]);
                    }
                    free(items[i].authors);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("Book: %s (%s) Authors:", items[i].title, items[i].year);
                    for (size_t j = 0; j < items[i].acount; j++) {
                        printf(" %s", items[i].authors[j]);
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].year);
        for (size_t j = 0; j < items[i].acount; j++) {
            free(items[i].authors[j]);
        }
        free(items[i].authors);
    }
    free(items);
    return 0;
}
