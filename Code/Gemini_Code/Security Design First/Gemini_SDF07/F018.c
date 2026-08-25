/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int freq;
} WordFreq;

int compare_freqs(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    if (wfB->freq != wfA->freq) {
        return wfB->freq - wfA->freq;
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char *argv[]) {
    WordFreq *table = NULL;
    int count = 0;
    int capacity = 0;

    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread_bytes;
            while ((fread_bytes = getline(&fline, &flen, fp)) != -1) {
                fline[strcspn(fline, "\r\n")] = '\0';
                if (strlen(fline) == 0) continue;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].word, fline) == 0) {
                        table[i].freq++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    char *n_word = strdup(fline);
                    if (n_word) {
                        if (count >= capacity) {
                            int new_cap = capacity == 0 ? 4 : capacity * 2;
                            WordFreq *tmp = realloc(table, new_cap * sizeof(WordFreq));
                            if (tmp) {
                                table = tmp;
                                capacity = new_cap;
                            } else {
                                free(n_word);
                                continue;
                            }
                        }
                        table[count].word = n_word;
                        table[count].freq = 1;
                        count++;
                    }
                }
            }
            free(fline);
            fclose(fp);
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
            char *w = line + 5;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    table[i].freq++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                char *n_word = strdup(w);
                if (n_word) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        WordFreq *tmp = realloc(table, new_cap * sizeof(WordFreq));
                        if (tmp) {
                            table = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_word);
                            continue;
                        }
                    }
                    table[count].word = n_word;
                    table[count].freq = 1;
                    count++;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *w = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    free(table[i].word);
                    for (int j = i; j < count - 1; j++) {
                        table[j] = table[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *w = line + 6;
            int f = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, w) == 0) {
                    f = table[i].freq;
                    break;
                }
            }
            printf("%d\n", f);
        } else if (strcmp(line, "TOP") == 0) {
            if (count > 0) {
                qsort(table, count, sizeof(WordFreq), compare_freqs);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", table[i].word, table[i].freq);
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
