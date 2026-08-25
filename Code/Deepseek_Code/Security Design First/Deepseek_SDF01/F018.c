// filename: F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WordNode {
    char *word;
    int count;
    struct WordNode *next;
} WordNode;

static WordNode *words = NULL;

static void cleanup(void) {
    while (words) {
        WordNode *tmp = words;
        words = words->next;
        free(tmp->word);
        free(tmp);
    }
}

static WordNode *find_word(const char *word) {
    for (WordNode *w = words; w; w = w->next)
        if (strcmp(w->word, word) == 0)
            return w;
    return NULL;
}

static void add_word(const char *word) {
    WordNode *w = find_word(word);
    if (w) {
        w->count++;
        return;
    }
    w = malloc(sizeof(WordNode));
    if (!w) return;
    w->word = strdup(word);
    if (!w->word) { free(w); return; }
    w->count = 1;
    w->next = words;
    words = w;
}

static void remove_word(const char *word) {
    WordNode *prev = NULL;
    for (WordNode *w = words; w; w = w->next) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else words = w->next;
            free(w->word);
            free(w);
            return;
        }
        prev = w;
    }
}

static void count_word(const char *word) {
    WordNode *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

static void top_words(void) {
    // count number of nodes
    int n = 0;
    for (WordNode *w = words; w; w = w->next) ++n;
    if (n == 0) return;
    // copy to array
    WordNode **arr = malloc(n * sizeof(WordNode *));
    if (!arr) return;
    int i = 0;
    for (WordNode *w = words; w; w = w->next) arr[i++] = w;
    // sort descending by count
    for (int j = 0; j < n - 1; ++j) {
        for (int k = 0; k < n - j - 1; ++k) {
            if (arr[k]->count < arr[k+1]->count) {
                WordNode *tmp = arr[k];
                arr[k] = arr[k+1];
                arr[k+1] = tmp;
            }
        }
    }
    for (int j = 0; j < n; ++j)
        printf("%s %d\n", arr[j]->word, arr[j]->count);
    free(arr);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = '\0';
                if (*line) add_word(line);
            }
            free(line);
            fclose(f);
        }
    }
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        if (strcmp(cmd, "WORD") == 0) {
            if (*arg1) add_word(arg1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*arg1) remove_word(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (*arg1) count_word(arg1);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free(line);
    return 0;
}