// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *word;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

Word *find_word(const char *word) {
    Word *w = words;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *word) {
    Word *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(Word));
        w->word = strdup(word);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(const char *word) {
    Word *w = find_word(word);
    if (!w) return;
    Word *cur = words, *prev = NULL;
    while (cur) {
        if (cur == w) {
            if (prev) prev->next = cur->next;
            else words = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(w->word);
    free(w);
}

void count_word(const char *word) {
    Word *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

int compare_words(const void *a, const void *b) {
    Word *A = *(Word **)a;
    Word *B = *(Word **)b;
    if (A->count != B->count) return B->count - A->count;
    return strcmp(A->word, B->word);
}

void top_words(void) {
    int count = 0;
    Word *cur = words;
    while (cur) { count++; cur = cur->next; }
    if (count == 0) return;
    Word **arr = malloc(count * sizeof(Word *));
    cur = words;
    for (int i = 0; i < count; i++) { arr[i] = cur; cur = cur->next; }
    qsort(arr, count, sizeof(Word *), compare_words);
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

void cleanup_words(void) {
    Word *w = words;
    while (w) {
        Word *tmp = w;
        w = w->next;
        free(tmp->word);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strlen(line) > 0) increment_word(line);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], text[1024];
        int n = sscanf(line, "%31s %1023[^\n]", cmd, text);
        if (n < 2) continue;
        if (strcmp(cmd, "WORD") == 0) {
            increment_word(text);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_word(text);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_word(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    cleanup_words();
    return 0;
}