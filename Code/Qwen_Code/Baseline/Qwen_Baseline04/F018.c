// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} Word;

Word *words = NULL;
int wcount = 0;

int find_word(const char *word) {
    for (int i = 0; i < wcount; i++) {
        if (strcmp(words[i].word, word) == 0) return i;
    }
    return -1;
}

int cmp(const void *a, const void *b) {
    Word *x = (Word*)a;
    Word *y = (Word*)b;
    if (x->count != y->count) return y->count - x->count;
    return strcmp(x->word, y->word);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1000];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *token = strtok(line, " \t");
                while (token) {
                    int idx = find_word(token);
                    if (idx != -1) {
                        words[idx].count++;
                    } else {
                        words = realloc(words, sizeof(Word) * (wcount + 1));
                        words[wcount].word = strdup(token);
                        words[wcount].count = 1;
                        wcount++;
                    }
                    token = strtok(NULL, " \t");
                }
            }
            fclose(f);
        }
    }
    
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "WORD") == 0) {
            char text[1000];
            scanf(" %[^\n]", text);
            char *token = strtok(text, " \t");
            while (token) {
                int idx = find_word(token);
                if (idx != -1) {
                    words[idx].count++;
                } else {
                    words = realloc(words, sizeof(Word) * (wcount + 1));
                    words[wcount].word = strdup(token);
                    words[wcount].count = 1;
                    wcount++;
                }
                token = strtok(NULL, " \t");
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char text[1000];
            scanf(" %[^\n]", text);
            int idx = find_word(text);
            if (idx != -1) {
                free(words[idx].word);
                for (int i = idx; i < wcount - 1; i++) words[i] = words[i+1];
                wcount--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char text[1000];
            scanf(" %[^\n]", text);
            int idx = find_word(text);
            if (idx != -1) {
                printf("%d\n", words[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(words, wcount, sizeof(Word), cmp);
            for (int i = 0; i < wcount; i++) {
                printf("%s %d\n", words[i].word, words[i].count);
            }
        }
    }
    for (int i = 0; i < wcount; i++) {
        free(words[i].word);
    }
    free(words);
    return 0;
}