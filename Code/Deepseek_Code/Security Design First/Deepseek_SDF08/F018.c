// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct wfreq {
    char *word;
    int count;
    struct wfreq *next;
} WFreq;

WFreq *words = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

WFreq *find_word(const char *word) {
    WFreq *w = words;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void add_word(const char *word) {
    WFreq *w = find_word(word);
    if (w) {
        w->count++;
        return;
    }
    WFreq *new = malloc(sizeof(WFreq));
    if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->word = strdup(word);
    if (!new->word) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->count = 1;
    new->next = words;
    words = new;
}

void remove_word(const char *word) {
    WFreq *w = words, *prev = NULL;
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
    WFreq *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

int cmp_freq(const void *a, const void *b) {
    WFreq *wa = *(WFreq**)a;
    WFreq *wb = *(WFreq**)b;
    return wb->count - wa->count; // descending
}

void top_words(void) {
    int count = 0;
    WFreq *w = words;
    while (w) { count++; w = w->next; }
    if (count == 0) return;
    WFreq **arr = malloc(count * sizeof(WFreq*));
    if (!arr) { fprintf(stderr, "Memory error\n"); exit(1); }
    w = words;
    for (int i = 0; i < count; i++) { arr[i] = w; w = w->next; }
    qsort(arr, count, sizeof(WFreq*), cmp_freq);
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

void free_all(void) {
    WFreq *w = words;
    while (w) {
        WFreq *next = w->next;
        free(w->word);
        free(w);
        w = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[MAX_LINE];
            while (fgets(line, sizeof(line), f)) {
                trim_newline(line);
                if (strlen(line) > 0) add_word(line);
            }
            fclose(f);
        }
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "TOP") == 0) { top_words(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "WORD") == 0) {
            add_word(rest);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_word(rest);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_word(rest);
        }
    }
    free_all();
    return 0;
}