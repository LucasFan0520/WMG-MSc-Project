// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *word; int count; } Word;
Word *words = 0; int n = 0, cap = 0;

int find(char *word) {
    for (int i=0; i<n; i++) if (!strcmp(words[i].word, word)) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = 0;
        int i = find(line);
        if (i >= 0) words[i].count++;
        else {
            if (n == cap) { cap = cap ? cap*2 : 4; words = realloc(words, cap*sizeof(Word)); }
            words[n].word = strdup(line); words[n].count = 1; n++;
        }
    }
    fclose(f);
}

int cmp(const void *a, const void *b) {
    return ((Word*)b)->count - ((Word*)a)->count;
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], text[1000];
        text[0] = 0;
        sscanf(line, "%s %[^\n]", cmd, text);
        if (!strcmp(cmd, "WORD")) {
            int i = find(text);
            if (i >= 0) words[i].count++;
            else {
                if (n == cap) { cap = cap ? cap*2 : 4; words = realloc(words, cap*sizeof(Word)); }
                words[n].word = strdup(text); words[n].count = 1; n++;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            int i = find(text);
            if (i >= 0) {
                free(words[i].word);
                for (int j=i; j<n-1; j++) words[j] = words[j+1]; n--;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            int i = find(text); printf("%d\n", i >= 0 ? words[i].count : 0);
        } else if (!strcmp(cmd, "TOP")) {
            qsort(words, n, sizeof(Word), cmp);
            for (int i=0; i<n; i++) printf("%s %d\n", words[i].word, words[i].count);
        }
    }
    return 0;
}