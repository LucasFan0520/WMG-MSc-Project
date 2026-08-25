// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char **body_lines;
    size_t body_count;
} CatalogRecord;

int main(int argc, char **argv) {
    CatalogRecord *catalog = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *tline = read_line(f);
                if (!tline) break;
                char *title = strdup(tline);
                free(tline);
                char **body = NULL;
                size_t b_count = 0;
                char *bline = NULL;
                while ((bline = read_line(f)) != NULL) {
                    if (strcmp(bline, "--") == 0) {
                        free(bline);
                        break;
                    }
                    body = realloc(body, (b_count + 1) * sizeof(char *));
                    body[b_count++] = bline;
                }
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    CatalogRecord *new_catalog = realloc(catalog, capacity * sizeof(CatalogRecord));
                    if (new_catalog) catalog = new_catalog;
                }
                if (count < capacity) {
                    catalog[count].title = title;
                    catalog[count].body_lines = body;
                    catalog[count].body_count = b_count;
                    count++;
                }
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    printf("%s\n", catalog[i].title);
                    for (size_t k = 0; k < catalog[i].body_count; k++) {
                        printf("%s\n", catalog[i].body_lines[k]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (size_t k = 0; k < catalog[i].body_count; k++) {
                        free(catalog[i].body_lines[k]);
                    }
                    free(catalog[i].body_lines);
                    for (size_t j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = strdup(p);
            char **body = NULL;
            size_t b_count = 0;
            char *bline = NULL;
            while ((bline = read_line(stdin)) != NULL) {
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                body = realloc(body, (b_count + 1) * sizeof(char *));
                body[b_count++] = bline;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                CatalogRecord *new_catalog = realloc(catalog, capacity * sizeof(CatalogRecord));
                if (new_catalog) catalog = new_catalog;
            }
            if (count < capacity) {
                catalog[count].title = title;
                catalog[count].body_lines = body;
                catalog[count].body_count = b_count;
                count++;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (size_t k = 0; k < catalog[i].body_count; k++) {
                    printf("%s\n", catalog[i].body_lines[k]);
                }
                printf("--\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(catalog[i].title);
        for (size_t k = 0; k < catalog[i].body_count; k++) {
            free(catalog[i].body_lines[k]);
        }
        free(catalog[i].body_lines);
    }
    free(catalog);
    return 0;
}
