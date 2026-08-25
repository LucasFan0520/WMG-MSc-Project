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
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char **body;
    int body_count;
} CatalogRecord;

int main(int argc, char **argv) {
    CatalogRecord *arr = NULL;
    int count = 0;
    int cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline;
            int in_rec = 0;
            char *cur_title = NULL;
            char **cur_body = NULL;
            int cur_body_cnt = 0;
            while ((fline = read_line(f)) != NULL) {
                char *trl = fline;
                size_t l = strlen(trl);
                while (l > 0 && (trl[l-1] == '\n' || trl[l-1] == '\n')) {
                    trl[l-1] = '\0';
                    l--;
                }
                if (strcmp(trl, "--") == 0) {
                    if (in_rec && cur_title) {
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            CatalogRecord *narr = realloc(arr, cap * sizeof(CatalogRecord));
                            if (narr) arr = narr;
                        }
                        if (count < cap) {
                            arr[count].title = cur_title;
                            arr[count].body = cur_body;
                            arr[count].body_count = cur_body_cnt;
                            count++;
                        }
                    }
                    in_rec = 0;
                    cur_title = NULL;
                    cur_body = NULL;
                    cur_body_cnt = 0;
                } else {
                    if (!in_rec) {
                        cur_title = mystrdup(trl);
                        in_rec = 1;
                    } else {
                        cur_body = realloc(cur_body, (cur_body_cnt + 1) * sizeof(char *));
                        if (cur_body) {
                            cur_body[cur_body_cnt] = mystrdup(trl);
                            cur_body_cnt++;
                        }
                    }
                }
                free(fline);
            }
            if (in_rec && cur_title) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    CatalogRecord *narr = realloc(arr, cap * sizeof(CatalogRecord));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].title = cur_title;
                    arr[count].body = cur_body;
                    arr[count].body_count = cur_body_cnt;
                    count++;
                }
            }
            fclose(f);
        }
    }
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '	' || *p == '\n' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            size_t tl = strlen(title);
            while (tl > 0 && (title[tl-1] == '\n' || title[tl-1] == '\n')) {
                title[tl-1] = '\0';
                tl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("Title: %s\n", arr[i].title);
                    for (int j = 0; j < arr[i].body_count; j++) {
                        printf("%s\n", arr[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            size_t tl = strlen(title);
            while (tl > 0 && (title[tl-1] == '\n' || title[tl-1] == '\n')) {
                title[tl-1] = '\0';
                tl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    for (int j = 0; j < arr[i].body_count; j++) {
                        free(arr[i].body[j]);
                    }
                    free(arr[i].body);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            size_t tl = strlen(title);
            while (tl > 0 && (title[tl-1] == '\n' || title[tl-1] == '\n')) {
                title[tl-1] = '\0';
                tl--;
            }
            char *ntitle = mystrdup(title);
            char **nbody = NULL;
            int nbody_cnt = 0;
            char *bline;
            while ((bline = read_line(stdin)) != NULL) {
                char *bt = bline;
                size_t bl = strlen(bt);
                while (bl > 0 && (bt[bl-1] == '\n' || bt[bl-1] == '\n')) {
                    bt[bl-1] = '\0';
                    bl--;
                }
                if (strcmp(bt, "--") == 0) {
                    free(bline);
                    break;
                }
                nbody = realloc(nbody, (nbody_cnt + 1) * sizeof(char *));
                if (nbody) {
                    nbody[nbody_cnt] = mystrdup(bt);
                    nbody_cnt++;
                }
                free(bline);
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                CatalogRecord *narr = realloc(arr, cap * sizeof(CatalogRecord));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].title = ntitle;
                arr[count].body = nbody;
                arr[count].body_count = nbody_cnt;
                count++;
            } else {
                free(ntitle);
                for (int j = 0; j < nbody_cnt; j++) free(nbody[j]);
                free(nbody);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Title: %s\n", arr[i].title);
                for (int j = 0; j < arr[i].body_count; j++) {
                    printf("%s\n", arr[i].body[j]);
                }
                printf("--\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        for (int j = 0; j < arr[i].body_count; j++) {
            free(arr[i].body[j]);
        }
        free(arr[i].body);
    }
    free(arr);
    return 0;
}
