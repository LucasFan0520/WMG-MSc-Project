/* F018.c */
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
    char *word;
    long long count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    const WordFreq *ra = (const WordFreq *)a;
    const WordFreq *rb = (const WordFreq *)b;
    if (ra->count > rb->count) return -1;
    if (ra->count < rb->count) return 1;
    return strcmp(ra->word, rb->word);
}

int main(int argc, char **argv) {
    WordFreq *table = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char *p = line;
                while (*p == ' ') p++;
                if (*p != '\0') {
                    char *end = p + strlen(p);
                    while (end > p && *(end - 1) == ' ') end--;
                    *end = '\0';
                    int found = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(table[i].word, p) == 0) {
                            found = (int)i;
                            break;
                        }
                    }
                    if (found != -1) {
                        table[found].count++;
                    } else {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            WordFreq *nt = realloc(table, cap * sizeof(WordFreq));
                            if (nt) table = nt;
                        }
                        char *nw = mystrdup(p);
                        if (nw) {
                            table[count].word = nw;
                            table[count].count = 1;
                            count++;
                        }
                    }
                }
                free(line);
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].word, text) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                table[found].count++;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    WordFreq *nt = realloc(table, cap * sizeof(WordFreq));
                    if (nt) table = nt;
                }
                char *nw = mystrdup(text);
                if (nw) {
                    table[count].word = nw;
                    table[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].word, text) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(table[found].word);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    table[i] = table[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].word, text) == 0) {
                    cnt = table[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(table, count, sizeof(WordFreq), compare_freq);
                for (size_t i = 0; i < count; i++) {
                    printf("%s %lld\n", table[i].word, table[i].count);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(table[i].word);
    }
    free(table);
    return 0;
}
