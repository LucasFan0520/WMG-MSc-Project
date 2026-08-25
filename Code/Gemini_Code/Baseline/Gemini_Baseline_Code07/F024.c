// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    int year;
    char **authors;
    int auth_count;
} Book;

int main() {
    Book *bib = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s (%d)\n", bib[i].title, bib[i].year);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "BOOK") == 0) {
            char title[256];
            int year = 0;
            if (sscanf(p, "%255s %d", title, &year) == 2) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    bib = realloc(bib, capacity * sizeof(Book));
                }
                bib[count].title = strdup(title);
                bib[count].year = year;
                bib[count].authors = NULL;
                bib[count].auth_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char title[256], auth[256];
            if (sscanf(p, "%255s %255s", title, auth) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(bib[i].title, title) == 0) {
                        bib[i].authors = realloc(bib[i].authors, (bib[i].auth_count + 1) * sizeof(char *));
                        bib[i].authors[bib[i].auth_count++] = strdup(auth);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char title[256], auth[256];
            if (sscanf(p, "%255s %255s", title, auth) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(bib[i].title, title) == 0) {
                        int found_auth = -1;
                        for (int j = 0; j < bib[i].auth_count; j++) {
                            if (strcmp(bib[i].authors[j], auth) == 0) {
                                found_auth = j;
                                break;
                            }
                        }
                        if (found_auth != -1) {
                            free(bib[i].authors[found_auth]);
                            for (int j = found_auth; j < bib[i].auth_count - 1; j++) {
                                bib[i].authors[j] = bib[i].authors[j + 1];
                            }
                            bib[i].auth_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(bib[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(bib[found].title);
                    for (int j = 0; j < bib[found].auth_count; j++) {
                        free(bib[found].authors[j]);
                    }
                    free(bib[found].authors);
                    for (int i = found; i < count - 1; i++) {
                        bib[i] = bib[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(bib[i].title, title) == 0) {
                        for (int j = 0; j < bib[i].auth_count; j++) {
                            printf("%s%s", bib[i].authors[j], (j == bib[i].auth_count - 1) ? "" : ", ");
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(bib[i].title);
        for (int j = 0; j < bib[i].auth_count; j++) {
            free(bib[i].authors[j]);
        }
        free(bib[i].authors);
    }
    free(bib);
    return 0;
}
