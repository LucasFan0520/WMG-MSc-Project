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
    const WordFreq *pa = (const WordFreq *)a;
    const WordFreq *pb = (const WordFreq *)b;
    if (pa->count > pb->count) return -1;
    if (pa->count < pb->count) return 1;
    return strcmp(pa->word, pb->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while ((read = getline(&line, &len, f)) != -1) {
                if (read > 0 && line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                if (strlen(line) > 0) {
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].word, line) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        list[found].count++;
                    } else {
                        WordFreq *tmp = realloc(list, sizeof(WordFreq) * (count + 1));
                        if (tmp) {
                            list = tmp;
                            list[count].word = strdup(line);
                            list[count].count = 1;
                            if (list[count].word) {
                                count++;
                            }
                        }
                    }
                }
            }
            fclose(f);
        }
    }

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *word = line + 5;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, word) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                list[found].count++;
            } else {
                WordFreq *tmp = realloc(list, sizeof(WordFreq) * (count + 1));
                if (tmp) {
                    list = tmp;
                    list[count].word = strdup(word);
                    list[count].count = 1;
                    if (list[count].word) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *word = line + 7;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, word) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].word);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *word = line + 6;
            long long cnt = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, word) == 0) {
                    cnt = list[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(line, "TOP") == 0) {
            if (count > 0) {
                qsort(list, count, sizeof(WordFreq), compare_freq);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %lld\n", list[i].word, list[i].count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    free(line);
    return 0;
}
