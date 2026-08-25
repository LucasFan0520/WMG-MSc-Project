/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WordCount {
    char *word;
    int count;
    struct WordCount *next;
} WordCount;

WordCount *words = NULL;

WordCount *find_word(const char *word) {
    WordCount *w = words;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *word) {
    WordCount *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(WordCount));
        w->word = strdup(word);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(const char *word) {
    WordCount *w = words, *prev = NULL;
    while (w && strcmp(w->word, word) != 0) {
        prev = w;
        w = w->next;
    }
    if (!w) return;
    if (prev) prev->next = w->next;
    else words = w->next;
    free(w->word);
    free(w);
}

void count_word(const char *word) {
    WordCount *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

void top_words(void) {
    int count = 0;
    WordCount *w = words;
    while (w) { count++; w = w->next; }
    if (count == 0) return;
    WordCount **arr = malloc(count * sizeof(WordCount *));
    w = words;
    for (int i = 0; i < count; i++) {
        arr[i] = w;
        w = w->next;
    }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->count < arr[j]->count ||
                (arr[i]->count == arr[j]->count && strcmp(arr[i]->word, arr[j]->word) > 0)) {
                WordCount *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

void free_all(void) {
    WordCount *w = words;
    while (w) {
        WordCount *next = w->next;
        free(w->word);
        free(w);
        w = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                increment_word(line);
            }
            fclose(f);
        }
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            increment_word(p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_word(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            count_word(p);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    free(line);
    return 0;
}