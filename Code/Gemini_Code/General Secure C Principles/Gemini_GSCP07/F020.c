// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} CatalogRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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

int main(int argc, char **argv) {
    CatalogRec *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
                size_t bsize = 16;
                size_t blen = 0;
                char *body = malloc(bsize);
                body[0] = '\0';
                while (1) {
                    char *bline = read_line(fp);
                    if (!bline || strcmp(bline, "--") == 0) {
                        free(bline);
                        break;
                    }
                    size_t nlen = strlen(bline);
                    if (blen + nlen + 2 >= bsize) {
                        bsize = bsize * 2 + nlen;
                        char *nb = realloc(body, bsize);
                        if (nb) body = nb;
                    }
                    if (blen > 0) {
                        strcat(body, "\n");
                        blen++;
                    }
                    strcat(body, bline);
                    blen += nlen;
                    free(bline);
                }
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    CatalogRec *nl = realloc(list, capacity * sizeof(CatalogRec));
                    if (!nl) {
                        free(title);
                        free(body);
                        break;
                    }
                    list = nl;
                }
                list[count].title = title;
                list[count].body = body;
                count++;
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
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
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    printf("%s\n", list[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(list[i].title);
                    free(list[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t bsize = 16;
            size_t blen = 0;
            char *body = malloc(bsize);
            body[0] = '\0';
            while (1) {
                char *bline = read_line(stdin);
                if (!bline || strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                size_t nlen = strlen(bline);
                if (blen + nlen + 2 >= bsize) {
                    bsize = bsize * 2 + nlen;
                    char *nb = realloc(body, bsize);
                    if (nb) body = nb;
                }
                if (blen > 0) {
                    strcat(body, "\n");
                    blen++;
                }
                strcat(body, bline);
                blen += nlen;
                free(bline);
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                CatalogRec *nl = realloc(list, capacity * sizeof(CatalogRec));
                if (!nl) {
                    free(body);
                    break;
                }
                list = nl;
            }
            list[count].title = strdup(title);
            list[count].body = body;
            count++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n%s\n--\n", list[i].title, list[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].body);
    }
    free(list);
    return 0;
}
