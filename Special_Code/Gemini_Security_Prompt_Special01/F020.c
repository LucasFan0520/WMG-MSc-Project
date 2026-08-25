// F020.c
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
    char **body_lines;
    size_t line_count;
    size_t line_capacity;
} CatalogRecord;

int main(int argc, char **argv) {
    CatalogRecord *catalog = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            int new_record = 1;
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                if (strcmp(fline, "--") == 0) {
                    new_record = 1;
                    free(fline);
                    continue;
                }
                if (new_record) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        CatalogRecord *new_cat = realloc(catalog, capacity * sizeof(CatalogRecord));
                        if (new_cat) catalog = new_cat;
                    }
                    catalog[count].title = mystrdup(fline);
                    catalog[count].body_lines = NULL;
                    catalog[count].line_count = 0;
                    catalog[count].line_capacity = 0;
                    count++;
                    new_record = 0;
                } else {
                    CatalogRecord *curr = &catalog[count - 1];
                    if (curr->line_count >= curr->line_capacity) {
                        curr->line_capacity = curr->line_capacity == 0 ? 4 : curr->line_capacity * 2;
                        char **new_b = realloc(curr->body_lines, curr->line_capacity * sizeof(char *));
                        if (new_b) curr->body_lines = new_b;
                    }
                    curr->body_lines[curr->line_count++] = mystrdup(fline);
                }
                free(fline);
            }
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
        if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    for (size_t j = 0; j < catalog[i].line_count; j++) {
                        printf("%s\n", catalog[i].body_lines[j]);
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
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (size_t j = 0; j < catalog[i].line_count; j++) {
                        free(catalog[i].body_lines[j]);
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
            char *title = p;
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                CatalogRecord *new_cat = realloc(catalog, capacity * sizeof(CatalogRecord));
                if (new_cat) catalog = new_cat;
            }
            catalog[count].title = mystrdup(title);
            catalog[count].body_lines = NULL;
            catalog[count].line_count = 0;
            catalog[count].line_capacity = 0;
            count++;
            CatalogRecord *curr = &catalog[count - 1];
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                if (curr->line_count >= curr->line_capacity) {
                    curr->line_capacity = curr->line_capacity == 0 ? 4 : curr->line_capacity * 2;
                    char **new_b = realloc(curr->body_lines, curr->line_capacity * sizeof(char *));
                    if (new_b) curr->body_lines = new_b;
                }
                curr->body_lines[curr->line_count++] = mystrdup(bline);
                free(bline);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (size_t j = 0; j < catalog[i].line_count; j++) {
                    printf("%s\n", catalog[i].body_lines[j]);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(catalog[i].title);
        for (size_t j = 0; j < catalog[i].line_count; j++) {
            free(catalog[i].body_lines[j]);
        }
        free(catalog[i].body_lines);
    }
    free(catalog);
    return 0;
}
