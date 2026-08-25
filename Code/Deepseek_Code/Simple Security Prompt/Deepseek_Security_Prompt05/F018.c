/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

Word *find_word(const char *text) {
    Word *w = words;
    while (w) {
        if (strcmp(w->text, text) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void inc_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
        return;
    }
    w = malloc(sizeof(Word));
    if (!w) return;
    w->text = strdup(text);
    w->count = 1;
    w->next = words;
    words = w;
}

void remove_word(const char *text) {
    Word *w = words, *prev = NULL;
    while (w) {
        if (strcmp(w->text, text) == 0) {
            if (prev) prev->next = w->next;
            else words = w->next;
            free(w->text);
            free(w);
            return;
        }
        prev = w;
        w = w->next;
    }
}

void count_word(const char *text) {
    Word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

int compare_word(const void *a, const void *b) {
    Word *wa = *(Word**)a, *wb = *(Word**)b;
    if (wa->count != wb->count) return wb->count - wa->count;
    return strcmp(wa->text, wb->text);
}

void top_words(void) {
    int cnt = 0;
    Word *w = words;
    while (w) { cnt++; w = w->next; }
    if (!cnt) return;
    Word **arr = malloc(cnt * sizeof(Word*));
    if (!arr) return;
    int i = 0;
    w = words;
    while (w) { arr[i++] = w; w = w->next; }
    qsort(arr, cnt, sizeof(Word*), compare_word);
    for (i = 0; i < cnt; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                inc_word(line);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg[2048];
        int n = sscanf(line, "%15s %2047s", cmd, arg);
        if (n < 2) continue;
        if (strcmp(cmd, "WORD") == 0) {
            inc_word(arg);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_word(arg);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_word(arg);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    while (words) {
        Word *tmp = words;
        words = words->next;
        free(tmp->text);
        free(tmp);
    }
    return 0;
}