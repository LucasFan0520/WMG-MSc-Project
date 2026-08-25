// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_word_freq(const void *a, const void *b) {
    const WordFreq *aa = (const WordFreq *)a;
    const WordFreq *bb = (const WordFreq *)b;
    if (aa->count > bb->count) return -1;
    if (aa->count < bb->count) return 1;
    return strcmp(aa->word, bb->word);
}

int main(int argc, char **argv) {
    WordFreq *table = NULL;
    int count = 0;
    int capacity = 0;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, f)) != -1) {
                if (fread > 0 && fline[fread - 1] == '\n') fline[fread - 1] = '\0';
                char *w = fline;
                while (*w == ' ') w++;
                if (*w == '\0') continue;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].word, w) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    table[found].count++;
                } else {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        WordFreq *tmp = realloc(table, new_cap * sizeof(WordFreq));
                        if (!tmp) return 1;
                        table = tmp;
                        capacity = new_cap;
                    }
                    table[count].word = strdup(w);
                    table[count].count = 1;
                    count++;
                }
            }
            free(fline);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "WORD ", 5) == 0) {
            char *w = line + 5;
            while (*w == ' ') w++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                table[found].count++;
            } else {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *tmp = realloc(table, new_cap * sizeof(WordFreq));
                    if (!tmp) return 1;
                    table = tmp;
                    capacity = new_cap;
                }
                table[count].word = strdup(w);
                table[count].count = 1;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *w = line + 7;
            while (*w == ' ') w++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(table[found].word);
                for (int i = found; i < count - 1; i++) {
                    table[i] = table[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *w = line + 6;
            while (*w == ' ') w++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%d\n", table[found].count);
            else printf("0\n");
        } else if (strcmp(line, "TOP") == 0) {
            if (count > 0) {
                qsort(table, count, sizeof(WordFreq), compare_word_freq);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", table[i].word, table[i].count);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(table[i].word);
    }
    free(table);
    return 0;
}
