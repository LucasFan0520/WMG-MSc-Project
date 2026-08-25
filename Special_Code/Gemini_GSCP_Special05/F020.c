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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *title;
    char **body;
    size_t lines_count;
    size_t lines_cap;
} CatalogRecord;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    CatalogRecord *cat = NULL;
    size_t count = 0;
    size_t cap = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        int next_is_title = 1;
        size_t cur_idx = (size_t)-1;
        while (1) {
            char *fline = read_line(fp);
            if (!fline) break;
            if (strcmp(fline, "--") == 0) {
                next_is_title = 1;
            } else {
                if (next_is_title) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        CatalogRecord *nb = realloc(cat, cap * sizeof(CatalogRecord));
                        if (!nb) {
                            free(fline);
                            break;
                        }
                        cat = nb;
                    }
                    cat[count].title = safe_dup(fline);
                    cat[count].body = NULL;
                    cat[count].lines_count = 0;
                    cat[count].lines_cap = 0;
                    cur_idx = count;
                    count++;
                    next_is_title = 0;
                } else {
                    if (cur_idx != (size_t)-1) {
                        CatalogRecord *r = &cat[cur_idx];
                        if (r->lines_count >= r->lines_cap) {
                            r->lines_cap = r->lines_cap == 0 ? 4 : r->lines_cap * 2;
                            char **nb = realloc(r->body, r->lines_cap * sizeof(char *));
                            if (!nb) break;
                            r->body = nb;
                        }
                        r->body[r->lines_count] = safe_dup(fline);
                        r->lines_count++;
                    }
                }
            }
            free(fline);
        }
        fclose(fp);
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cat[i].title, title) == 0) {
                    for (size_t j = 0; j < cat[i].lines_count; j++) {
                        printf("%s\n", cat[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cat[i].title, title) == 0) {
                    free(cat[i].title);
                    for (size_t j = 0; j < cat[i].lines_count; j++) {
                        free(cat[i].body[j]);
                    }
                    free(cat[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        cat[j] = cat[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                CatalogRecord *nb = realloc(cat, cap * sizeof(CatalogRecord));
                if (!nb) {
                    free(line);
                    break;
                }
                cat = nb;
            }
            cat[count].title = safe_dup(title);
            cat[count].body = NULL;
            cat[count].lines_count = 0;
            cat[count].lines_cap = 0;
            size_t cur_idx = count;
            count++;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                CatalogRecord *r = &cat[cur_idx];
                if (r->lines_count >= r->lines_cap) {
                    r->lines_cap = r->lines_cap == 0 ? 4 : r->lines_cap * 2;
                    char **nb = realloc(r->body, r->lines_cap * sizeof(char *));
                    if (!nb) {
                        free(bline);
                        break;
                    }
                    r->body = nb;
                }
                r->body[r->lines_count] = safe_dup(bline);
                r->lines_count++;
                free(bline);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", cat[i].title);
                for (size_t j = 0; j < cat[i].lines_count; j++) {
                    printf("%s\n", cat[i].body[j]);
                }
                if (i < count - 1) printf("--\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(cat[i].title);
        for (size_t j = 0; j < cat[i].lines_count; j++) {
            free(cat[i].body[j]);
        }
        free(cat[i].body);
    }
    free(cat);
    return 0;
}
