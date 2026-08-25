// F018.c
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
    char *word;
    int count;
} WordFreq;

int cmp_freq(const void *a, const void *b) {
    WordFreq *aa = (WordFreq *)a;
    WordFreq *bb = (WordFreq *)b;
    if (aa->count > bb->count) return -1;
    if (aa->count < bb->count) return 1;
    return strcmp(aa->word, bb->word);
}

int main(int argc, char **argv) {
    WordFreq *arr = NULL;
    int count = 0;
    int cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline;
            while ((fline = read_line(f)) != NULL) {
                char *w = fline;
                size_t wl = strlen(w);
                while (wl > 0 && (w[wl-1] == '\n' || w[wl-1] == '\n')) {
                    w[wl-1] = '\0';
                    wl--;
                }
                if (wl > 0) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].word, w) == 0) {
                            arr[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            WordFreq *narr = realloc(arr, cap * sizeof(WordFreq));
                            if (narr) arr = narr;
                        }
                        if (count < cap) {
                            arr[count].word = mystrdup(w);
                            arr[count].count = 1;
                            count++;
                        }
                    }
                }
                free(fline);
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
        if (strcmp(cmd, "WORD") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *w = p;
            size_t wl = strlen(w);
            while (wl > 0 && (w[wl-1] == '\n' || w[wl-1] == '\n')) {
                w[wl-1] = '\0';
                wl--;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    arr[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    WordFreq *narr = realloc(arr, cap * sizeof(WordFreq));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].word = mystrdup(w);
                    arr[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *w = p;
            size_t wl = strlen(w);
            while (wl > 0 && (w[wl-1] == '\n' || w[wl-1] == '\n')) {
                w[wl-1] = '\0';
                wl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    free(arr[i].word);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *w = p;
            size_t wl = strlen(w);
            while (wl > 0 && (w[wl-1] == '\n' || w[wl-1] == '\n')) {
                w[wl-1] = '\0';
                wl--;
            }
            int cnt = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    cnt = arr[i].count;
                    break;
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(WordFreq), cmp_freq);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", arr[i].word, arr[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].word);
    }
    free(arr);
    return 0;
}
