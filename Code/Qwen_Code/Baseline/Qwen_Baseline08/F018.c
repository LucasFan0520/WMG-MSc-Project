// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 10000

typedef struct {
    char word[100];
    int count;
} Word;

Word words[MAX_WORDS];
int wcount = 0;

int find_word(const char *word) {
    for (int i = 0; i < wcount; i++) if (strcmp(words[i].word, word) == 0) return i;
    return -1;
}

int cmp(const void *a, const void *b) {
    return ((Word *)b)->count - ((Word *)a)->count;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[100];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (line[0]) {
                    int idx = find_word(line);
                    if (idx == -1 && wcount < MAX_WORDS) {
                        strcpy(words[wcount].word, line);
                        words[wcount].count = 1;
                        wcount++;
                    } else if (idx != -1) {
                        words[idx].count++;
                    }
                }
            }
            fclose(f);
        }
    }
    char cmd[20], text[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "WORD") == 0) {
            scanf("%s", text);
            int idx = find_word(text);
            if (idx == -1 && wcount < MAX_WORDS) {
                strcpy(words[wcount].word, text);
                words[wcount].count = 1;
                wcount++;
            } else if (idx != -1) {
                words[idx].count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", text);
            int idx = find_word(text);
            if (idx != -1) {
                for (int i = idx; i < wcount - 1; i++) words[i] = words[i + 1];
                wcount--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", text);
            int idx = find_word(text);
            printf("%d\n", idx != -1 ? words[idx].count : 0);
        } else if (strcmp(cmd, "TOP") == 0) {
            Word temp[MAX_WORDS];
            memcpy(temp, words, sizeof(Word) * wcount);
            qsort(temp, wcount, sizeof(Word), cmp);
            for (int i = 0; i < wcount; i++) printf("%s %d\n", temp[i].word, temp[i].count);
        }
    }
    return 0;
}