/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} Word;

static Word *words = NULL;
static size_t wcount = 0, wcap = 0;

static Word *find_word(const char *word) {
    for (size_t i = 0; i < wcount; i++) {
        if (strcmp(words[i].word, word) == 0)
            return &words[i];
    }
    return NULL;
}

static void add_word(const char *word, int cnt) {
    Word *w = find_word(word);
    if (w) {
        w->count += cnt;
    } else {
        if (wcount == wcap) {
            wcap = wcap ? wcap * 2 : 4;
            words = realloc(words, wcap * sizeof(Word));
            if (!words) { perror("realloc"); exit(1); }
        }
        words[wcount].word = strdup(word);
        words[wcount].count = cnt;
        wcount++;
    }
}

static void remove_word(const char *word) {
    for (size_t i = 0; i < wcount; i++) {
        if (strcmp(words[i].word, word) == 0) {
            free(words[i].word);
            for (size_t j = i; j < wcount - 1; j++)
                words[j] = words[j + 1];
            wcount--;
            return;
        }
    }
}

static void count_word(const char *word) {
    Word *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

static int cmp_desc(const void *a, const void *b) {
    const Word *wa = (const Word *)a;
    const Word *wb = (const Word *)b;
    if (wb->count != wa->count) return wb->count - wa->count;
    return strcmp(wa->word, wb->word);
}

static void top_words(void) {
    qsort(words, wcount, sizeof(Word), cmp_desc);
    for (size_t i = 0; i < wcount; i++) {
        printf("%s %d\n", words[i].word, words[i].count);
    }
}

static void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) {
            add_word(line, 1);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *word = strtok(NULL, "");
            if (!word) word = "";
            add_word(word, 1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *word = strtok(NULL, " ");
            if (word) remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *word = strtok(NULL, " ");
            if (word) count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    for (size_t i = 0; i < wcount; i++) {
        free(words[i].word);
    }
    free(words);
    return 0;
}