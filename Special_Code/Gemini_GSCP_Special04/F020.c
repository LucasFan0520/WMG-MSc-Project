/* F020.c */
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char **body;
    size_t b_count;
    size_t b_cap;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Record *catalog = NULL;
    size_t count = 0;
    size_t cap = 0;
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
                Record *nc = realloc(catalog, cap * sizeof(Record));
                if (nc) catalog = nc;
            }
            catalog[count].title = title;
            catalog[count].body = NULL;
            catalog[count].b_count = 0;
            catalog[count].b_cap = 0;
            Record *r = &catalog[count];
            count++;
            while (1) {
                char *bline = read_line(fp);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                if (r->b_count >= r->b_cap) {
                    r->b_cap = r->b_cap == 0 ? 4 : r->b_cap * 2;
                    char **nb = realloc(r->body, r->b_cap * sizeof(char *));
                    if (nb) r->body = nb;
                }
                r->body[r->b_count++] = bline;
            }
        }
        fclose(fp);
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", catalog[found].title);
                for (size_t j = 0; j < catalog[found].b_count; j++) {
                    printf("%s\n", catalog[found].body[j]);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(catalog[found].title);
                for (size_t j = 0; j < catalog[found].b_count; j++) {
                    free(catalog[found].body[j]);
                }
                free(catalog[found].body);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    catalog[i] = catalog[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Record *nc = realloc(catalog, cap * sizeof(Record));
                if (nc) catalog = nc;
            }
            char *nt = mystrdup(title);
            if (nt) {
                catalog[count].title = nt;
                catalog[count].body = NULL;
                catalog[count].b_count = 0;
                catalog[count].b_cap = 0;
                Record *r = &catalog[count];
                count++;
                while (1) {
                    char *bline = read_line(stdin);
                    if (!bline) break;
                    if (strcmp(bline, "--") == 0) {
                        free(bline);
                        break;
                    }
                    if (r->b_count >= r->b_cap) {
                        r->b_cap = r->b_cap == 0 ? 4 : r->b_cap * 2;
                        char **nb = realloc(r->body, r->b_cap * sizeof(char *));
                        if (nb) r->body = nb;
                    }
                    r->body[r->b_count++] = bline;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (size_t j = 0; j < catalog[i].b_count; j++) {
                    printf("%s\n", catalog[i].body[j]);
                }
                if (i + 1 < count) printf("--\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(catalog[i].title);
        for (size_t j = 0; j < catalog[i].b_count; j++) {
            free(catalog[i].body[j]);
        }
        free(catalog[i].body);
    }
    free(catalog);
    return 0;
}
