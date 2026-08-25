// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *word; int count; } WF;
WF *freq = NULL;
int freq_count = 0, freq_cap = 0;

int find_word(const char *word) {
    for (int i = 0; i < freq_count; i++)
        if (strcmp(freq[i].word, word) == 0) return i;
    return -1;
}

void inc_word(const char *word) {
    int idx = find_word(word);
    if (idx >= 0) {
        freq[idx].count++;
    } else {
        if (freq_count == freq_cap) {
            freq_cap = freq_cap ? freq_cap * 2 : 4;
            freq = realloc(freq, sizeof(WF) * freq_cap);
        }
        freq[freq_count].word = strdup(word);
        freq[freq_count].count = 1;
        freq_count++;
    }
}

void remove_word(const char *word) {
    int idx = find_word(word);
    if (idx < 0) return;
    free(freq[idx].word);
    for (int i = idx; i < freq_count - 1; i++) freq[i] = freq[i + 1];
    freq_count--;
}

void count_word(const char *word) {
    int idx = find_word(word);
    printf("%d\n", idx >= 0 ? freq[idx].count : 0);
}

int cmp_freq(const void *a, const void *b) {
    const WF *A = (const WF *)a, *B = (const WF *)b;
    return B->count - A->count;
}

void top_words(void) {
    qsort(freq, freq_count, sizeof(WF), cmp_freq);
    for (int i = 0; i < freq_count; i++)
        printf("%s %d\n", freq[i].word, freq[i].count);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char word[4096];
    while (fgets(word, sizeof(word), f)) {
        word[strcspn(word, "\n")] = '\0';
        inc_word(word);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], word[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            inc_word(p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %4095s", word) == 1) remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %4095s", word) == 1) count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) top_words();
    }
    for (int i = 0; i < freq_count; i++) free(freq[i].word);
    free(freq);
    return 0;
}