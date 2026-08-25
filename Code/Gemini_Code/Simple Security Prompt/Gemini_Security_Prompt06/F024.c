// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *year;
    char **authors;
    int a_count;
} Book;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Book *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s (%s)\n", list[i].title, list[i].year);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "BOOK") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = rest;
                char *year = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Book));
                list[count].title = strdup(title);
                list[count].year = strdup(year);
                list[count].authors = NULL;
                list[count].a_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0 || strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = rest;
                char *name = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    if (strcmp(cmd, "AUTHOR") == 0) {
                        int ac = list[found].a_count;
                        list[found].authors = realloc(list[found].authors, (ac + 1) * sizeof(char *));
                        list[found].authors[ac] = strdup(name);
                        list[found].a_count++;
                    } else {
                        int a_idx = -1;
                        for (int i = 0; i < list[found].a_count; i++) {
                            if (strcmp(list[found].authors[i], name) == 0) {
                                a_idx = i;
                                break;
                            }
                        }
                        if (a_idx != -1) {
                            free(list[found].authors[a_idx]);
                            for (int i = a_idx; i < list[found].a_count - 1; i++) {
                                list[found].authors[i] = list[found].authors[i + 1];
                            }
                            list[found].a_count--;
                            if (list[found].a_count == 0) {
                                free(list[found].authors);
                                list[found].authors = NULL;
                            } else {
                                list[found].authors = realloc(list[found].authors, list[found].a_count * sizeof(char *));
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].title);
                free(list[found].year);
                for (int j = 0; j < list[found].a_count; j++) {
                    free(list[found].authors[j]);
                }
                free(list[found].authors);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Book));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    for (int j = 0; j < list[i].a_count; j++) {
                        printf("%s%s", list[i].authors[j], (j == list[i].a_count - 1) ? "" : " ");
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].year);
        for (int j = 0; j < list[i].a_count; j++) {
            free(list[i].authors[j]);
        }
        free(list[i].authors);
    }
    free(list);
    return 0;
}