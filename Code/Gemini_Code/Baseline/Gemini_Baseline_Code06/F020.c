// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *title;
    char **body_lines;
    int body_count;
} CatalogRecord;

int main(int argc, char *argv[]) {
    CatalogRecord *catalog = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            int need_title = 1;
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                if (need_title) {
                    catalog = realloc(catalog, (count + 1) * sizeof(CatalogRecord));
                    catalog[count].title = strdup(line);
                    catalog[count].body_lines = NULL;
                    catalog[count].body_count = 0;
                    need_title = 0;
                } else {
                    if (strcmp(line, "--") == 0) {
                        count++;
                        need_title = 1;
                    } else {
                        catalog[count].body_lines = realloc(catalog[count].body_lines, (catalog[count].body_count + 1) * sizeof(char *));
                        catalog[count].body_lines[catalog[count].body_count] = strdup(line);
                        catalog[count].body_count++;
                    }
                }
                free(line);
            }
            if (!need_title) count++;
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body_lines[j]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        free(catalog[i].body_lines[j]);
                    }
                    free(catalog[i].body_lines);
                    for (int j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    catalog = realloc(catalog, count * sizeof(CatalogRecord));
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            catalog = realloc(catalog, (count + 1) * sizeof(CatalogRecord));
            catalog[count].title = strdup(title);
            catalog[count].body_lines = NULL;
            catalog[count].body_count = 0;
            while (1) {
                char *b_line = read_line(stdin);
                if (!b_line || strcmp(b_line, "--") == 0) {
                    if (b_line) free(b_line);
                    break;
                }
                catalog[count].body_lines = realloc(catalog[count].body_lines, (catalog[count].body_count + 1) * sizeof(char *));
                catalog[count].body_lines[catalog[count].body_count] = strdup(b_line);
                catalog[count].body_count++;
                free(b_line);
            }
            count++;
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Title: %s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].body_count; j++) {
                    printf("%s\n", catalog[i].body_lines[j]);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].body_count; j++) {
            free(catalog[i].body_lines[j]);
        }
        free(catalog[i].body_lines);
    }
    free(catalog);
    return 0;
}
