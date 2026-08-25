// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    long long count;
} WordFreq;

int compare_freqs(const void *a, const void *b) {
    const WordFreq *wfA = (const WordFreq *)a;
    const WordFreq *wfB = (const WordFreq *)b;
    if (wfB->count != wfA->count) {
        return (wfB->count > wfA->count) ? 1 : -1;
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *table = NULL;
    size_t count = 0;
    size_t capacity = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while ((read = getline(&line, &len, fp)) != -1) {
                line[strcspn(line, "\r\n")] = '\0';
                char *w = line;
                while (*w == ' ' || *w == '\t') w++;
                if (*w == '\0') continue;
                char *end = w;
                while (*end && *end != ' ' && *end != '\t') end++;
                *end = '\0';

                int found_idx = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(table[i].word, w) == 0) {
                        found_idx = (int)i;
                        break;
                    }
                }
                if (found_idx != -1) {
                    table[found_idx].count++;
                } else {
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        WordFreq *new_t = realloc(table, new_cap * sizeof(WordFreq));
                        if (!new_t) continue;
                        table = new_t;
                        capacity = new_cap;
                    }
                    char *nw = strdup(w);
                    if (nw) {
                        table[count].word = nw;
                        table[count].count = 1;
                        count++;
                    }
                }
            }
            fclose(fp);
        }
    }

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*w == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }
            if (found_idx != -1) {
                table[found_idx].count++;
            } else {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *new_t = realloc(table, new_cap * sizeof(WordFreq));
                    if (!new_t) continue;
                    table = new_t;
                    capacity = new_cap;
                }
                char *nw = strdup(w);
                if (nw) {
                    table[count].word = nw;
                    table[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*w == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    free(table[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        table[j] = table[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*w == '\0') continue;

            long long cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    cnt = table[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 1) {
                qsort(table, count, sizeof(WordFreq), compare_freqs);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", table[i].word, table[i].count);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(table[i].word);
    }
    free(table);
    free(line);
    return 0;
}
