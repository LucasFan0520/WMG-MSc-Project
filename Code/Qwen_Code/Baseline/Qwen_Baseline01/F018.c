// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} Word;

Word *words = NULL;
int n = 0;

int find_word(const char *word) {
    for (int i = 0; i < n; i++)
        if (strcmp(words[i].word, word) == 0) return i;
    return -1;
}

void load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        int i = find_word(line);
        if (i != -1) words[i].count++;
        else {
            words = realloc(words, sizeof(Word) * (n + 1));
            words[n].word = strdup(line);
            words[n].count = 1;
            n++;
        }
    }
    fclose(f);
}

int cmp(const void *a, const void *b) {
    return ((Word*)b)->count - ((Word*)a)->count;
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 5;
            if (*p == ' ') p++;
            int i = find_word(p);
            if (i != -1) words[i].count++;
            else {
                words = realloc(words, sizeof(Word) * (n + 1));
                words[n].word = strdup(p);
                words[n].count = 1;
                n++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            if (*p == ' ') p++;
            int i = find_word(p);
            if (i != -1) {
                free(words[i].word);
                for (int j = i; j < n - 1; j++) words[j] = words[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            if (*p == ' ') p++;
            int i = find_word(p);
            printf("%d\n", i != -1 ? words[i].count : 0);
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(words, n, sizeof(Word), cmp);
            for (int i = 0; i < n; i++) printf("%s %d\n", words[i].word, words[i].count);
        }
    }
    for (int i = 0; i < n; i++) free(words[i].word);
    free(words);
    return 0;
}