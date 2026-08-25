// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freqs(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    return wfB->count - wfA->count;
}

int main(int argc, char **argv) {
    WordFreq *freqs = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[4096];
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\r\n")] = 0;
                if (strlen(fline) > 0) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(freqs[i].word, fline) == 0) {
                            freqs[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        freqs = realloc(freqs, (count + 1) * sizeof(WordFreq));
                        freqs[count].word = strdup(fline);
                        freqs[count].count = 1;
                        count++;
                    }
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            char *text = strtok(NULL, " ");
            if (text) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, text) == 0) {
                        freqs[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    freqs = realloc(freqs, (count + 1) * sizeof(WordFreq));
                    freqs[count].word = strdup(text);
                    freqs[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *text = strtok(NULL, " ");
            if (text) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, text) == 0) {
                        free(freqs[i].word);
                        for (int j = i; j < count - 1; j++) {
                            freqs[j] = freqs[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(freqs);
                    freqs = NULL;
                } else {
                    freqs = realloc(freqs, count * sizeof(WordFreq));
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *text = strtok(NULL, " ");
            if (text) {
                int cnt = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, text) == 0) {
                        cnt = freqs[i].count;
                        break;
                    }
                }
                printf("%d\n", cnt);
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(freqs, count, sizeof(WordFreq), compare_freqs);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", freqs[i].word, freqs[i].count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(freqs[i].word);
    }
    free(freqs);
    return 0;
}
