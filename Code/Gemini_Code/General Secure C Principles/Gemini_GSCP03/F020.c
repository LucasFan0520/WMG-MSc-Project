// F020.c
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char **lines;
    int line_count;
    int line_cap;
} CatalogRecord;

CatalogRecord *catalog = NULL;
int cat_count = 0;
int cat_cap = 0;

void add_record_line(CatalogRecord *rec, char *line) {
    if (rec->line_count >= rec->line_cap) {
        rec->line_cap = rec->line_cap == 0 ? 16 : rec->line_cap * 2;
        char **nb = realloc(rec->lines, rec->line_cap * sizeof(char *));
        if (nb) rec->lines = nb;
    }
    rec->lines[rec->line_count] = strdup(line);
    rec->line_count++;
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *title = read_line(fp);
                if (!title) break;
                size_t tl = strlen(title);
                if (tl > 0 && title[tl - 1] == '\r') title[tl - 1] = '\0';
                if (cat_count >= cat_cap) {
                    cat_cap = cat_cap == 0 ? 16 : cat_cap * 2;
                    CatalogRecord *nb = realloc(catalog, cat_cap * sizeof(CatalogRecord));
                    if (nb) catalog = nb;
                }
                catalog[cat_count].title = strdup(title);
                catalog[cat_count].lines = NULL;
                catalog[cat_count].line_count = 0;
                catalog[cat_count].line_cap = 0;
                while (1) {
                    char *bline = read_line(fp);
                    if (!bline) break;
                    size_t bl = strlen(bline);
                    if (bl > 0 && bline[bl - 1] == '\r') bline[bl - 1] = '\0';
                    if (strcmp(bline, "--") == 0) {
                        free(bline);
                        break;
                    }
                    add_record_line(&catalog[cat_count], bline);
                    free(bline);
                }
                cat_count++;
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
        if (*p == '\0') {
            free(line);
            continue;
        }
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
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < cat_count; i++) {
                printf("%s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].line_count; j++) {
                    printf("%s\n", catalog[i].lines[j]);
                }
                printf("--\n");
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *title = p;
            if (cat_count >= cat_cap) {
                cat_cap = cat_cap == 0 ? 16 : cat_cap * 2;
                CatalogRecord *nb = realloc(catalog, cat_cap * sizeof(CatalogRecord));
                if (nb) catalog = nb;
            }
            catalog[cat_count].title = strdup(title);
            catalog[cat_count].lines = NULL;
            catalog[cat_count].line_count = 0;
            catalog[cat_count].line_cap = 0;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                add_record_line(&catalog[cat_count], bline);
                free(bline);
            }
            cat_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = p;
            for (int i = 0; i < cat_count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (int j = 0; j < catalog[i].line_count; j++) {
                        free(catalog[i].lines[j]);
                    }
                    free(catalog[i].lines);
                    for (int j = i; j < cat_count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    cat_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            for (int i = 0; i < cat_count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    for (int j = 0; j < catalog[i].line_count; j++) {
                        printf("%s\n", catalog[i].lines[j]);
                    }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < cat_count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].line_count; j++) {
            free(catalog[i].lines[j]);
        }
        free(catalog[i].lines);
    }
    free(catalog);
    return 0;
}
