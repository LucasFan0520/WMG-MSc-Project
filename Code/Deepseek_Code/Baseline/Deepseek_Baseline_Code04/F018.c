/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *word;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

Word *find_word(const char *text) {
    Word *w = words;
    while (w) {
        if (strcmp(w->word, text) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
    } else {
        Word *nw = malloc(sizeof(Word));
        nw->word = strdup(text);
        nw->count = 1;
        nw->next = words;
        words = nw;
    }
}

void remove_word(const char *text) {
    Word *prev = NULL, *cur = words;
    while (cur) {
        if (strcmp(cur->word, text) == 0) {
            if (prev) prev->next = cur->next;
            else words = cur->next;
            free(cur->word);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void count_word(const char *text) {
    Word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

int compare_count_desc(const void *a, const void *b) {
    Word **wa = (Word **)a, **wb = (Word **)b;
    return (*wb)->count - (*wa)->count;
}

void top_words(void) {
    int n = 0;
    Word *w = words;
    while (w) { n++; w = w->next; }
    if (n == 0) return;
    Word **arr = malloc(n * sizeof(Word *));
    w = words;
    for (int i = 0; i < n; i++) {
        arr[i] = w;
        w = w->next;
    }
    qsort(arr, n, sizeof(Word *), compare_count_desc);
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

void free_words(void) {
    while (words) {
        Word *tmp = words;
        words = words->next;
        free(tmp->word);
        free(tmp);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                if (*line) increment_word(line);
            }
            fclose(f);
        }
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char text[4096];
        int n = sscanf(line, "%s", cmd);
        if (n < 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (*p) increment_word(p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (*p) remove_word(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (*p) count_word(p);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_words();
    return 0;
}