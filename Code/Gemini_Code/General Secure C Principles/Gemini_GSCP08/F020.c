// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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
    char **body;
    size_t body_count;
    size_t body_cap;
} CatalogRecord;

void add_record(CatalogRecord **arr, size_t *count, size_t *cap, CatalogRecord rec) {
    if (*count >= *cap) {
        size_t ncap = *cap == 0 ? 4 : *cap * 2;
        CatalogRecord *narr = realloc(*arr, ncap * sizeof(CatalogRecord));
        if (narr) {
            *arr = narr;
            *cap = ncap;
        }
    }
    if (*count < *cap) {
        (*arr)[*count] = rec;
        (*count)++;
    }
}

int main(int argc, char **argv) {
    CatalogRecord *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            int parsing = 0;
            CatalogRecord cur;
            while (1) {
                char *fline = read_line(fp);
                if (!fline) {
                    if (parsing) {
                        add_record(&arr, &count, &cap, cur);
                    }
                    break;
                }
                size_t wl = strlen(fline);
                while (wl > 0 && (fline[wl - 1] == '\n' || fline[wl - 1] == '\r')) {
                    fline[wl - 1] = '\0';
                    wl--;
                }
                if (strcmp(fline, "--") == 0) {
                    if (parsing) {
                        add_record(&arr, &count, &cap, cur);
                        parsing = 0;
                    }
                    free(fline);
                    continue;
                }
                if (!parsing) {
                    cur.title = strdup(fline);
                    cur.body = NULL;
                    cur.body_count = 0;
                    cur.body_cap = 0;
                    parsing = 1;
                } else {
                    if (cur.body_count >= cur.body_cap) {
                        size_t ncap = cur.body_cap == 0 ? 4 : cur.body_cap * 2;
                        char **nbody = realloc(cur.body, ncap * sizeof(char *));
                        if (nbody) {
                            cur.body = nbody;
                            cur.body_cap = ncap;
                        }
                    }
                    if (cur.body_count < cur.body_cap) {
                        cur.body[cur.body_count] = strdup(fline);
                        cur.body_count++;
                    }
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
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", arr[i].title);
                    for (size_t j = 0; j < arr[i].body_count; j++) {
                        printf("%s\n", arr[i].body[j]);
                    }
                    printf("--\n");
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            CatalogRecord cur;
            cur.title = strdup(args);
            cur.body = NULL;
            cur.body_count = 0;
            cur.body_cap = 0;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                if (cur.body_count >= cur.body_cap) {
                    size_t ncap = cur.body_cap == 0 ? 4 : cur.body_cap * 2;
                    char **nbody = realloc(cur.body, ncap * sizeof(char *));
                    if (nbody) {
                        cur.body = nbody;
                        cur.body_cap = ncap;
                    }
                }
                if (cur.body_count < cur.body_cap) {
                    cur.body[cur.body_count] = strdup(bline);
                    cur.body_count++;
                }
                free(bline);
            }
            add_record(&arr, &count, &cap, cur);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    for (size_t j = 0; j < arr[i].body_count; j++) {
                        free(arr[i].body[j]);
                    }
                    free(arr[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    for (size_t j = 0; j < arr[i].body_count; j++) {
                        printf("%s\n", arr[i].body[j]);
                    }
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        for (size_t j = 0; j < arr[i].body_count; j++) {
            free(arr[i].body[j]);
        }
        free(arr[i].body);
    }
    free(arr);
    return 0;
}
