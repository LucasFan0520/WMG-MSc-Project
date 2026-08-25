// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *title;
    char **body;
    int bcount;
    int bcap;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Record *arr = NULL;
    int count = 0;
    int cap = 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        int new_rec = 1;
        while (1) {
            char *line = read_line(f);
            if (!line) break;
            if (strcmp(line, "--") == 0) {
                new_rec = 1;
            } else {
                if (new_rec) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Record *narr = realloc(arr, cap * sizeof(Record));
                        if (narr) arr = narr;
                    }
                    arr[count].title = strdup(line);
                    arr[count].body = NULL;
                    arr[count].bcount = 0;
                    arr[count].bcap = 0;
                    new_rec = 0;
                    count++;
                } else {
                    Record *r = &arr[count - 1];
                    if (r->bcount >= r->bcap) {
                        r->bcap = r->bcap == 0 ? 4 : r->bcap * 2;
                        char **nbody = realloc(r->body, r->bcap * sizeof(char *));
                        if (nbody) r->body = nbody;
                    }
                    r->body[r->bcount] = strdup(line);
                    r->bcount++;
                }
            }
            free(line);
        }
        fclose(f);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "FIND") == 0) {
            char *title = get_rest(&p);
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("%s\n", arr[i].title);
                    for (int j = 0; j < arr[i].bcount; j++) {
                        printf("%s\n", arr[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = get_rest(&p);
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    for (int j = 0; j < arr[i].bcount; j++) {
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
            char *title = get_rest(&p);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Record *narr = realloc(arr, cap * sizeof(Record));
                if (narr) arr = narr;
            }
            arr[count].title = strdup(title);
            arr[count].body = NULL;
            arr[count].bcount = 0;
            arr[count].bcap = 0;
            count++;
            Record *r = &arr[count - 1];
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                if (r->bcount >= r->bcap) {
                    r->bcap = r->bcap == 0 ? 4 : r->bcap * 2;
                    char **nbody = realloc(r->body, r->bcap * sizeof(char *));
                    if (nbody) r->body = nbody;
                }
                r->body[r->bcount] = strdup(bline);
                r->bcount++;
                free(bline);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].title);
                for (int j = 0; j < arr[i].bcount; j++) {
                    printf("%s\n", arr[i].body[j]);
                }
                if (i < count - 1) {
                    printf("--\n");
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        for (int j = 0; j < arr[i].bcount; j++) {
            free(arr[i].body[j]);
        }
        free(arr[i].body);
    }
    free(arr);
    return 0;
}
