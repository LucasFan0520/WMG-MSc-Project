// F018.c
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
    char *word;
    long long count;
} WordFreq;

int cmp_freq(const void *a, const void *b) {
    const WordFreq *pa = (const WordFreq *)a;
    const WordFreq *pb = (const WordFreq *)b;
    if (pa->count > pb->count) return -1;
    if (pa->count < pb->count) return 1;
    return strcmp(pa->word, pb->word);
}

int main(int argc, char **argv) {
    WordFreq *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line_from(fp);
                if (!line) break;
                char *w = line;
                size_t wlen = strlen(w);
                while (wlen > 0 && (w[wlen-1] == '\n' || w[wlen-1] == '\n')) {
                    w[wlen-1] = '\0';
                    wlen--;
                }
                if (wlen > 0) {
                    int found = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(items[i].word, w) == 0) {
                            found = (int)i;
                            break;
                        }
                    }
                    if (found != -1) {
                        items[found].count++;
                    } else {
                        if (count >= cap) {
                            cap = cap == 0 ? 8 : cap * 2;
                            WordFreq *nitems = realloc(items, cap * sizeof(WordFreq));
                            if (nitems) items = nitems;
                        }
                        items[count].word = mystrdup(w);
                        items[count].count = 1;
                        count++;
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
        if (strcmp(cmd, "WORD") == 0) {
            char *w = p;
            size_t wlen = strlen(w);
            while (wlen > 0 && (w[wlen-1] == '\n' || w[wlen-1] == '\n')) {
                w[wlen-1] = '\0';
                wlen--;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, w) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                items[found].count++;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    WordFreq *nitems = realloc(items, cap * sizeof(WordFreq));
                    if (nitems) items = nitems;
                }
                items[count].word = mystrdup(w);
                items[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *w = p;
            size_t wlen = strlen(w);
            while (wlen > 0 && (w[wlen-1] == '\n' || w[wlen-1] == '\n')) {
                w[wlen-1] = '\0';
                wlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].word, w) == 0) {
                    free(items[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *w = p;
            size_t wlen = strlen(w);
            while (wlen > 0 && (w[wlen-1] == '\n' || w[wlen-1] == '\n')) {
                w[wlen-1] = '\0';
                wlen--;
            }
            long long cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, w) == 0) {
                    cval = items[i].count;
                    break;
                }
            }
            printf("%lld\n", cval);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 1) {
                qsort(items, count, sizeof(WordFreq), cmp_freq);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", items[i].word, items[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].word);
    }
    free(items);
    return 0;
}
