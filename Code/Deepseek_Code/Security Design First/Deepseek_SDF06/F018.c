// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

static WordFreq *words = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_word(const char *word) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(words[i].word, word) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_word(const char *word) {
    ssize_t idx = find_word(word);
    if (idx != -1) {
        words[idx].count++;
        return;
    }
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        WordFreq *new = realloc(words, capacity * sizeof(WordFreq));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        words = new;
    }
    words[count].word = strdup(word);
    if (!words[count].word) { fprintf(stderr, "Memory error\n"); exit(1); }
    words[count].count = 1;
    count++;
}

static void remove_word(const char *word) {
    ssize_t idx = find_word(word);
    if (idx == -1) return;
    free(words[idx].word);
    for (size_t i = idx; i < count - 1; i++)
        words[i] = words[i + 1];
    count--;
}

static void count_word(const char *word) {
    ssize_t idx = find_word(word);
    printf("%d\n", idx == -1 ? 0 : words[idx].count);
}

static int cmp_desc(const void *a, const void *b) {
    const WordFreq *wa = (const WordFreq *)a;
    const WordFreq *wb = (const WordFreq *)b;
    return wb->count - wa->count;
}

static void top(void) {
    qsort(words, count, sizeof(WordFreq), cmp_desc);
    for (size_t i = 0; i < count; i++)
        printf("%s %d\n", words[i].word, words[i].count);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++)
        free(words[i].word);
    free(words);
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, f)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        add_word(line);
    }
    free(line);
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);

    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], word[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "WORD") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", word) == 1)
                add_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", word) == 1)
                remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", word) == 1)
                count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            top();
        }
    }

    free(line);
    free_all();
    return 0;
}