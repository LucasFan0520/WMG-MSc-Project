// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} Word;

Word words[10000];
int wc = 0;

int find_word(char *word) {
    for (int i = 0; i < wc; i++) if (strcmp(words[i].word, word) == 0) return i;
    return -1;
}

int cmp(const void *a, const void *b) {
    Word *x = (Word *)a;
    Word *y = (Word *)b;
    return y->count - x->count;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        int idx = find_word(line);
        if (idx == -1) {
            words[wc].word = strdup(line);
            words[wc].count = 1;
            wc++;
        } else {
            words[idx].count++;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "WORD") == 0) {
            int idx = find_word(rest);
            if (idx == -1) {
                words[wc].word = strdup(rest);
                words[wc].count = 1;
                wc++;
            } else {
                words[idx].count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = find_word(rest);
            if (idx != -1) {
                free(words[idx].word);
                for (int i = idx; i < wc - 1; i++) words[i] = words[i+1];
                wc--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int idx = find_word(rest);
            if (idx != -1) printf("%d\n", words[idx].count);
            else printf("0\n");
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(words, wc, sizeof(Word), cmp);
            for (int i = 0; i < wc; i++) printf("%s %d\n", words[i].word, words[i].count);
        }
    }
    for (int i = 0; i < wc; i++) free(words[i].word);
    return 0;
}