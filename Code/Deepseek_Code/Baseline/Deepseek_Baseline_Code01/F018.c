// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct wf {
    char *word;
    int count;
    struct wf *next;
} WF;

WF *head = NULL;
WF *tail = NULL;

WF *find_word(const char *word) {
    WF *w = head;
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
        return;
    }
    w = malloc(sizeof(WF));
    w->word = strdup(word);
    w->count = 1;
    w->next = NULL;
    if (tail) tail->next = w;
    else head = w;
    tail = w;
}

void remove_word(const char *word) {
    WF *w = head, *prev = NULL;
    while (w) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
            if (w == tail) tail = prev;
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

int cmp_desc(const void *a, const void *b) {
    WF *wa = *(WF**)a, *wb = *(WF**)b;
    return wb->count - wa->count;
}

void top_words(void) {
    int n = 0;
    WF *w = head;
    while (w) { n++; w = w->next; }
    if (n == 0) return;
    WF **arr = malloc(n * sizeof(WF*));
    w = head;
    for (int i = 0; i < n; i++, w = w->next) arr[i] = w;
    qsort(arr, n, sizeof(WF*), cmp_desc);
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        increment_word(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char word[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            if (sscanf(line, "%*s %[^\n]", word) == 1)
                increment_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %[^\n]", word) == 1)
                remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %[^\n]", word) == 1)
                count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    return 0;
}