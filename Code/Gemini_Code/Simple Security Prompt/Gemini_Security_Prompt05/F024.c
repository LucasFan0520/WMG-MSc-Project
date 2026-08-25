// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
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

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

typedef struct {
    char *title;
    char *year;
    char **authors;
    int acount;
    int acap;
} Book;

int main() {
    Book *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = get_token(&p);
            char *year = get_token(&p);
            if (title && year) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Book *narr = realloc(arr, cap * sizeof(Book));
                    if (narr) arr = narr;
                }
                arr[count].title = strdup(title);
                arr[count].year = strdup(year);
                arr[count].authors = NULL;
                arr[count].acount = 0;
                arr[count].acap = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = get_token(&p);
            char *name = get_token(&p);
            if (title && name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        Book *b = &arr[i];
                        if (b->acount >= b->acap) {
                            b->acap = b->acap == 0 ? 4 : b->acap * 2;
                            char **nauths = realloc(b->authors, b->acap * sizeof(char *));
                            if (nauths) b->authors = nauths;
                        }
                        b->authors[b->acount] = strdup(name);
                        b->acount++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = get_token(&p);
            char *name = get_token(&p);
            if (title && name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        for (int j = 0; j < arr[i].acount; j++) {
                            if (strcmp(arr[i].authors[j], name) == 0) {
                                free(arr[i].authors[j]);
                                for (int k = j; k < arr[i].acount - 1; k++) {
                                    arr[i].authors[k] = arr[i].authors[k + 1];
                                }
                                arr[i].acount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        free(arr[i].title);
                        free(arr[i].year);
                        for (int j = 0; j < arr[i].acount; j++) {
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
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = get_token(&p);
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        for (int j = 0; j < arr[i].acount; j++) {
                            printf("%s%s", arr[i].authors[j], j == arr[i].acount - 1 ? "" : " ");
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].title, arr[i].year);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].year);
        for (int j = 0; j < arr[i].acount; j++) {
            free(arr[i].authors[j]);
        }
        free(arr[i].authors);
    }
    free(arr);
    return 0;
}
