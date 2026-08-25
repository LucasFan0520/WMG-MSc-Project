// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    long long count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    const WordFreq *wfA = (const WordFreq *)a;
    const WordFreq *wfB = (const WordFreq *)b;
    if (wfA->count != wfB->count) {
        return wfA->count > wfB->count ? -1 : 1;
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, f)) != -1) {
                if (fread > 0 && fline[fread - 1] == '\n') {
                    fline[fread - 1] = '\0';
                }
                char *w = fline;
                while (*w == ' ') w++;
                if (*w == '\0') continue;
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].word, w) == 0) {
                        arr[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        WordFreq *tmp = realloc(arr, capacity * sizeof(WordFreq));
                        if (!tmp) exit(1);
                        arr = tmp;
                    }
                    arr[count].word = strdup(w);
                    arr[count].count = 1;
                    if (!arr[count].word) exit(1);
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
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *w = line + 5;
            while (*w == ' ') w++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    arr[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *tmp = realloc(arr, capacity * sizeof(WordFreq));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].word = strdup(w);
                arr[count].count = 1;
                if (!arr[count].word) exit(1);
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *w = line + 7;
            while (*w == ' ') w++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    free(arr[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *w = line + 6;
            while (*w == ' ') w++;
            long long c_val = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    c_val = arr[i].count;
                    break;
                }
            }
            printf("%lld\n", c_val);
        } else if (strcmp(line, "TOP") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(WordFreq), compare_freq);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", arr[i].word, arr[i].count);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].word);
    }
    free(arr);
    free(line);
    return 0;
}
