// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

WordFreq *table = NULL;
int count = 0;
int capacity = 0;

int find_word(const char *word) {
    for (int i = 0; i < count; i++) {
        if (strcmp(table[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

void add_word(const char *word) {
    int idx = find_word(word);
    if (idx != -1) {
        table[idx].count++;
    } else {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            table = realloc(table, capacity * sizeof(WordFreq));
        }
        table[count].word = strdup(word);
        table[count].count = 1;
        count++;
    }
}

int compare_freq(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    if (wfB->count != wfA->count) {
        return wfB->count - wfA->count;
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[1024];
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\r\n")] = 0;
                if (strlen(fline) > 0) {
                    add_word(fline);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            add_word(line + 5);
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *word = line + 7;
            int idx = find_word(word);
            if (idx != -1) {
                free(table[idx].word);
                for (int i = idx; i < count - 1; i++) {
                    table[i] = table[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *word = line + 6;
            int idx = find_word(word);
            if (idx != -1) {
                printf("%d\n", table[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(line, "TOP") == 0) {
            qsort(table, count, sizeof(WordFreq), compare_freq);
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", table[i].word, table[i].count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(table[i].word);
    }
    free(table);
    return 0;
}
