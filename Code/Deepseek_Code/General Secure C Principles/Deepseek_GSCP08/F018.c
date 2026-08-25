// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word { char *word; int count; struct Word *next; } Word;
Word *words = NULL;

Word *find_word(const char *word) {
    for (Word *w = words; w; w = w->next)
        if (strcmp(w->word, word) == 0) return w;
    return NULL;
}

void add_word(const char *word) {
    Word *w = find_word(word);
    if (w) { w->count++; return; }
    w = malloc(sizeof(Word));
    if (!w) return;
    w->word = strdup(word); w->count = 1; w->next = words; words = w;
}

void remove_word(const char *word) {
    Word *prev = NULL;
    for (Word *w = words; w; prev = w, w = w->next) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next; else words = w->next;
            free(w->word); free(w);
            return;
        }
    }
}

void count_word(const char *word) {
    Word *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

void top_words(void) {
    // collect into array and sort by count descending
    int n = 0;
    for (Word *w = words; w; w = w->next) n++;
    if (n == 0) return;
    Word **arr = malloc(n * sizeof(Word *));
    if (!arr) return;
    int i = 0;
    for (Word *w = words; w; w = w->next) arr[i++] = w;
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            if (arr[i]->count < arr[j]->count ||
                (arr[i]->count == arr[j]->count && strcmp(arr[i]->word, arr[j]->word) > 0)) {
                Word *tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < n; i++) printf("%s %d\n", arr[i]->word, arr[i]->count);
    free(arr);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char word[4096];
            while (fgets(word, sizeof(word), f)) {
                word[strcspn(word, "\n")] = 0;
                if (strlen(word) > 0) add_word(word);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "WORD") == 0) {
            char word[4096];
            strcpy(word, rest);
            add_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char word[4096];
            strcpy(word, rest);
            remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char word[4096];
            strcpy(word, rest);
            count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    while (words) { Word *w = words; words = w->next; free(w->word); free(w); }
    return 0;
}