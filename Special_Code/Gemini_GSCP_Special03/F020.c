// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line_from(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char **body;
    size_t bcount;
    size_t bcap;
} CatalogRecord;

int main(int argc, char **argv) {
    CatalogRecord *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            int need_title = 1;
            while (1) {
                char *line = read_line_from(fp);
                if (!line) break;
                size_t llen = strlen(line);
                while (llen > 0 && (line[llen-1] == '\n' || line[llen-1] == '\n')) {
                    line[llen-1] = '\0';
                    llen--;
                }
                if (strcmp(line, "--") == 0) {
                    need_title = 1;
                } else {
                    if (need_title) {
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            CatalogRecord *nitems = realloc(items, cap * sizeof(CatalogRecord));
                            if (nitems) items = nitems;
                        }
                        items[count].title = mystrdup(line);
                        items[count].body = NULL;
                        items[count].bcount = 0;
                        items[count].bcap = 0;
                        count++;
                        need_title = 0;
                    } else {
                        CatalogRecord *curr = &items[count-1];
                        if (curr->bcount >= curr->bcap) {
                            curr->bcap = curr->bcap == 0 ? 4 : curr->bcap * 2;
                            char **nb = realloc(curr->body, curr->bcap * sizeof(char*));
                            if (nb) curr->body = nb;
                        }
                        curr->body[curr->bcount] = mystrdup(line);
                        curr->bcount++;
                    }
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line_from(stdin);
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
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("%s\n", items[i].title);
                    for (size_t j = 0; j < items[i].bcount; j++) {
                        printf("%s\n", items[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    for (size_t j = 0; j < items[i].bcount; j++) {
                        free(items[i].body[j]);
                    }
                    free(items[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                CatalogRecord *nitems = realloc(items, cap * sizeof(CatalogRecord));
                if (nitems) items = nitems;
            }
            items[count].title = mystrdup(title);
            items[count].body = NULL;
            items[count].bcount = 0;
            items[count].bcap = 0;
            count++;
            CatalogRecord *curr = &items[count-1];
            while (1) {
                char *bline = read_line_from(stdin);
                if (!bline) break;
                size_t blen = strlen(bline);
                while (blen > 0 && (bline[blen-1] == '\n' || bline[blen-1] == '\n')) {
                    bline[blen-1] = '\0';
                    blen--;
                }
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                if (curr->bcount >= curr->bcap) {
                    curr->bcap = curr->bcap == 0 ? 4 : curr->bcap * 2;
                    char **nb = realloc(curr->body, curr->bcap * sizeof(char*));
                    if (nb) curr->body = nb;
                }
                curr->body[curr->bcount] = mystrdup(bline);
                curr->bcount++;
                free(bline);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (i > 0) printf("--\n");
                printf("%s\n", items[i].title);
                for (size_t j = 0; j < items[i].bcount; j++) {
                    printf("%s\n", items[i].body[j]);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        for (size_t j = 0; j < items[i].bcount; j++) {
            free(items[i].body[j]);
        }
        free(items[i].body);
    }
    free(items);
    return 0;
}
