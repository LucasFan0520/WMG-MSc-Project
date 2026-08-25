// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char **body;
    size_t body_count;
    size_t body_cap;
} CatalogRecord;

int main(int argc, char **argv) {
    CatalogRecord *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *title = read_line(fp);
                if (!title) break;
                if (strcmp(title, "--") == 0) {
                    free(title);
                    continue;
                }
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(CatalogRecord));
                }
                items[count].title = mystrdup(title);
                items[count].body_count = 0;
                items[count].body_cap = 4;
                items[count].body = malloc(items[count].body_cap * sizeof(char *));
                while (1) {
                    char *bline = read_line(fp);
                    if (!bline || strcmp(bline, "--") == 0) {
                        free(bline);
                        break;
                    }
                    if (items[count].body_count >= items[count].body_cap) {
                        items[count].body_cap *= 2;
                        items[count].body = realloc(items[count].body, items[count].body_cap * sizeof(char *));
                    }
                    items[count].body[items[count].body_count++] = mystrdup(bline);
                    free(bline);
                }
                count++;
                free(title);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *title = p;
        while (*p && *p != ' ') p++;
        if (*p) *p = '\0';
        if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("Title: %s\n", items[i].title);
                    for (size_t j = 0; j < items[i].body_count; j++) {
                        printf("%s\n", items[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    for (size_t j = 0; j < items[i].body_count; j++) {
                        free(items[i].body[j]);
                    }
                    free(items[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(CatalogRecord));
            }
            items[count].title = mystrdup(title);
            items[count].body_count = 0;
            items[count].body_cap = 4;
            items[count].body = malloc(items[count].body_cap * sizeof(char *));
            while (1) {
                char *bline = read_line(stdin);
                if (!bline || strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                if (items[count].body_count >= items[count].body_cap) {
                    items[count].body_cap *= 2;
                    items[count].body = realloc(items[count].body, items[count].body_cap * sizeof(char *));
                }
                items[count].body[items[count].body_count++] = mystrdup(bline);
                free(bline);
            }
            count++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Title: %s\n", items[i].title);
                for (size_t j = 0; j < items[i].body_count; j++) {
                    printf("%s\n", items[i].body[j]);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        for (size_t j = 0; j < items[i].body_count; j++) {
            free(items[i].body[j]);
        }
        free(items[i].body);
    }
    free(items);
    return 0;
}
