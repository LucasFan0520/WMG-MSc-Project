// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

Word* find_word(const char *text) {
    for (Word *w = words; w; w = w->next)
        if (strcmp(w->text, text) == 0) return w;
    return NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        Word *w = find_word(line);
        if (w) w->count++;
        else {
            w = malloc(sizeof(Word));
            if (!w) continue;
            w->text = strdup(line);
            w->count = 1;
            w->next = words;
            words = w;
        }
    }
    fclose(f);
}

int cmp_word(const void *a, const void *b) {
    const Word *wa = *(const Word **)a;
    const Word *wb = *(const Word **)b;
    return wb->count - wa->count;
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "WORD") == 0) {
            Word *w = find_word(rest);
            if (w) w->count++;
            else {
                w = malloc(sizeof(Word));
                if (!w) continue;
                w->text = strdup(rest);
                w->count = 1;
                w->next = words;
                words = w;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            Word *prev = NULL;
            for (Word *w = words; w; prev = w, w = w->next) {
                if (strcmp(w->text, rest) == 0) {
                    if (prev) prev->next = w->next;
                    else words = w->next;
                    free(w->text);
                    free(w);
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            Word *w = find_word(rest);
            printf("%d\n", w ? w->count : 0);
        } else if (strcmp(cmd, "TOP") == 0) {
            size_t count = 0;
            for (Word *w = words; w; w = w->next) count++;
            Word **arr = malloc(count * sizeof(Word*));
            if (!arr) continue;
            size_t idx = 0;
            for (Word *w = words; w; w = w->next) arr[idx++] = w;
            qsort(arr, count, sizeof(Word*), cmp_word);
            for (size_t i = 0; i < count; i++)
                printf("%s %d\n", arr[i]->text, arr[i]->count);
            free(arr);
        }
    }
    while (words) {
        Word *w = words;
        words = words->next;
        free(w->text);
        free(w);
    }
    return 0;
}