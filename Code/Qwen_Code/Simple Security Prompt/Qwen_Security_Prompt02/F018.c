// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

WordFreq *words = NULL;
int count = 0;

int find_word(const char *word) {
    for (int i = 0; i < count; i++) {
        if (strcmp(words[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

int cmp(const void *a, const void *b) {
    return ((WordFreq*)b)->count - ((WordFreq*)a)->count;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        int idx = find_word(line);
        if (idx != -1) {
            words[idx].count++;
        } else {
            words = realloc(words, sizeof(WordFreq) * (count + 1));
            words[count].word = strdup(line);
            words[count].count = 1;
            count++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char word[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "WORD") == 0) {
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            if (word[0] == ' ') {
                memmove(word, word + 1, strlen(word));
            }
            int idx = find_word(word);
            if (idx != -1) {
                words[idx].count++;
            } else {
                words = realloc(words, sizeof(WordFreq) * (count + 1));
                words[count].word = strdup(word);
                words[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            if (word[0] == ' ') {
                memmove(word, word + 1, strlen(word));
            }
            int idx = find_word(word);
            if (idx != -1) {
                free(words[idx].word);
                for (int i = idx; i < count - 1; i++) {
                    words[i] = words[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            if (word[0] == ' ') {
                memmove(word, word + 1, strlen(word));
            }
            int idx = find_word(word);
            if (idx != -1) {
                printf("%d\n", words[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(words, count, sizeof(WordFreq), cmp);
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", words[i].word, words[i].count);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(words[i].word);
    }
    free(words);

    return 0;
}