/*
 * F018.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct wf {
    char *word;
    int count;
    struct wf *next;
} WF;

WF *words = NULL;

WF *find_word(const char *word) {
    WF *w = words;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *word) {
    WF *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(WF));
        w->word = strdup(word);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(const char *word) {
    WF *prev = NULL, *w = words;
    while (w) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else words = w->next;
            free(w->word);
            free(w);
            return;
        }
        prev = w;
        w = w->next;
    }
}

void count_word(const char *word) {
    WF *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

int cmp_wf(const void *a, const void *b) {
    WF *wa = *(WF**)a, *wb = *(WF**)b;
    return wb->count - wa->count;
}

void top_words(void) {
    int n = 0;
    WF *w = words;
    while (w) { n++; w = w->next; }
    if (n == 0) return;
    WF **arr = malloc(sizeof(WF*) * n);
    w = words;
    for (int i = 0; i < n; i++, w = w->next) arr[i] = w;
    qsort(arr, n, sizeof(WF*), cmp_wf);
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char word[256];
            while (fgets(word, sizeof(word), f)) {
                word[strcspn(word, "\n")] = 0;
                increment_word(word);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], text[1024];
        if (sscanf(line, "%31s %1023s", cmd, text) < 1) continue;
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
    return 0;
}