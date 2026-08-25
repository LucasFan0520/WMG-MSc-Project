// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    size_t auth_count;
    size_t auth_capacity;
} Book;

int main(void) {
    Book *bib = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *year = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                year++;
            }
            while (*year == ' ') year++;
            char *year_end = year;
            while (*year_end && *year_end != ' ') year_end++;
            *year_end = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Book *new_b = realloc(bib, capacity * sizeof(Book));
                    if (new_b) bib = new_b;
                }
                bib[count].title = mystrdup(title);
                bib[count].year = mystrdup(year);
                bib[count].authors = NULL;
                bib[count].auth_count = 0;
                bib[count].auth_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *name = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                name++;
            }
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    Book *b = &bib[i];
                    if (b->auth_count >= b->auth_capacity) {
                        b->auth_capacity = b->auth_capacity == 0 ? 4 : b->auth_capacity * 2;
                        char **new_auths = realloc(b->authors, b->auth_capacity * sizeof(char *));
                        if (new_auths) b->authors = new_auths;
                    }
                    b->authors[b->auth_count++] = mystrdup(name);
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            char *name = title_end;
            if (*title_end != '\0') {
                *title_end = '\0';
                name++;
            }
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    Book *b = &bib[i];
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
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    free(bib[i].title);
                    free(bib[i].year);
                    for (size_t j = 0; j < bib[i].auth_count; j++) {
                        free(bib[i].authors[j]);
                    }
                    free(bib[i].authors);
                    for (size_t j = i; j < count - 1; j++) {
                        bib[j] = bib[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(bib[i].title, title) == 0) {
                    for (size_t j = 0; j < bib[i].auth_count; j++) {
                        printf("%s\n", bib[i].authors[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", bib[i].title, bib[i].year);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(bib[i].title);
        free(bib[i].year);
        for (size_t j = 0; j < bib[i].auth_count; j++) {
            free(bib[i].authors[j]);
        }
        free(bib[i].authors);
    }
    free(bib);
    return 0;
}
