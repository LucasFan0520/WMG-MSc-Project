// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freqs(const void *a, const void *b) {
    const WordFreq *aa = (const WordFreq *)a;
    const WordFreq *bb = (const WordFreq *)b;
    if (aa->count != bb->count) {
        return bb->count - aa->count;
    }
    return strcmp(aa->word, bb->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    size_t count = 0;
    size_t capacity = 0;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, f)) != -1) {
                fline[strcspn(fline, "\r\n")] = '\0';
                if (strlen(fline) > 0) {
                    int found = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(list[i].word, fline) == 0) {
                            found = (int)i;
                            break;
                        }
                    }
                    if (found != -1) {
                        list[found].count++;
                    } else {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            WordFreq *next = realloc(list, capacity * sizeof(WordFreq));
                            if (!next) break;
                            list = next;
                        }
                        list[count].word = strdup(fline);
                        list[count].count = 1;
                        count++;
                    }
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
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *word = line + 5;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, word) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                list[found].count++;
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *next = realloc(list, capacity * sizeof(WordFreq));
                    if (!next) break;
                    list = next;
                }
                list[count].word = strdup(word);
                list[count].count = 1;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *word = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, word) == 0) {
                    free(list[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *word = line + 6;
            int amt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, word) == 0) {
                    amt = list[i].count;
                    break;
                }
            }
            printf("%d\n", amt);
        } else if (strcmp(line, "TOP") == 0) {
            if (count > 0) {
                qsort(list, count, sizeof(WordFreq), compare_freqs);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", list[i].word, list[i].count);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    return 0;
}
