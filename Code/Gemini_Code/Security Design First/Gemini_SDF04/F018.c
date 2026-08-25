// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

WordFreq *freqs = NULL;
int count = 0;
int capacity = 0;

int find_word(const char *word) {
    for (int i = 0; i < count; i++) {
        if (strcmp(freqs[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

void add_word(const char *word) {
    int idx = find_word(word);
    if (idx != -1) {
        freqs[idx].count++;
    } else {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            WordFreq *tmp = realloc(freqs, capacity * sizeof(WordFreq));
            if (!tmp) return;
            freqs = tmp;
        }
        freqs[count].word = strdup(word);
        freqs[count].count = 1;
        if (freqs[count].word) {
            count++;
        }
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t nread;
            while ((nread = getline(&fline, &flen, f)) != -1) {
                size_t l = strlen(fline);
                if (l > 0 && fline[l - 1] == '\n') fline[l - 1] = '\0';
                if (strlen(fline) > 0) {
                    add_word(fline);
                }
            }
            free(fline);
            fclose(f);
        }
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') line[l - 1] = '\0';
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "WORD") == 0) {
                add_word(arg1);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                int idx = find_word(arg1);
                if (idx != -1) {
                    free(freqs[idx].word);
                    for (int i = idx; i < count - 1; i++) {
                        freqs[i] = freqs[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "COUNT") == 0) {
                int idx = find_word(arg1);
                if (idx != -1) {
                    printf("%d\n", freqs[idx].count);
                } else {
                    printf("0\n");
                }
            }
        } else {
            if (strcmp(cmd, "TOP") == 0) {
                for (int i = 0; i < count - 1; i++) {
                    for (int j = 0; j < count - i - 1; j++) {
                        if (freqs[j].count < freqs[j + 1].count) {
                            WordFreq tmp = freqs[j];
                            freqs[j] = freqs[j + 1];
                            freqs[j + 1] = tmp;
                        }
                    }
                }
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", freqs[i].word, freqs[i].count);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(freqs[i].word);
    }
    free(freqs);
    return 0;
}