// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_WORD 256

typedef struct WordCount {
    char word[MAX_WORD];
    int count;
    struct WordCount *next;
} WordCount;

WordCount *head = NULL;

WordCount *find_word(const char *word) {
    WordCount *w = head;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void free_all() {
    WordCount *w = head;
    while (w) {
        WordCount *tmp = w;
        w = w->next;
        free(tmp);
    }
    head = NULL;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *p = line;
        while (*p) {
            while (*p && isspace(*p)) p++;
            if (!*p) break;
            char word[MAX_WORD];
            int i = 0;
            while (*p && !isspace(*p) && i < MAX_WORD-1) {
                word[i++] = *p++;
            }
            word[i] = '\0';
            WordCount *w = find_word(word);
            if (w) {
                w->count++;
            } else {
                WordCount *nw = malloc(sizeof(WordCount));
                if (!nw) continue;
                strcpy(nw->word, word);
                nw->count = 1;
                nw->next = head;
                head = nw;
            }
        }
    }
    fclose(f);
}

void handle_word(char *line) {
    char word[MAX_WORD];
    if (sscanf(line, "WORD %255s", word) != 1) return;
    WordCount *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        WordCount *nw = malloc(sizeof(WordCount));
        if (!nw) return;
        strcpy(nw->word, word);
        nw->count = 1;
        nw->next = head;
        head = nw;
    }
}

void handle_remove(char *line) {
    char word[MAX_WORD];
    if (sscanf(line, "REMOVE %255s", word) != 1) return;
    WordCount *w = head, *prev = NULL;
    while (w) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
            free(w);
            return;
        }
        prev = w;
        w = w->next;
    }
}

void handle_count(char *line) {
    char word[MAX_WORD];
    if (sscanf(line, "COUNT %255s", word) != 1) return;
    WordCount *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

int compare_count(const void *a, const void *b) {
    WordCount **wa = (WordCount **)a;
    WordCount **wb = (WordCount **)b;
    return (*wb)->count - (*wa)->count;
}

void handle_top() {
    int count = 0;
    WordCount *w = head;
    while (w) { count++; w = w->next; }
    if (count == 0) return;
    WordCount **arr = malloc(count * sizeof(WordCount *));
    if (!arr) return;
    w = head;
    for (int i = 0; i < count; i++) {
        arr[i] = w;
        w = w->next;
    }
    qsort(arr, count, sizeof(WordCount *), compare_count);
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "WORD ", 5) == 0) handle_word(line);
        else if (strncmp(line, "REMOVE ", 7) == 0) handle_remove(line);
        else if (strncmp(line, "COUNT ", 6) == 0) handle_count(line);
        else if (strcmp(line, "TOP") == 0) handle_top();
    }
    free_all();
    return 0;
}