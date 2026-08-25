// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *head = NULL;

Word *find_word(const char *text) {
    Word *w = head;
    while (w) {
        if (strcmp(w->text, text) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
        return;
    }
    w = malloc(sizeof(Word));
    w->text = malloc(strlen(text) + 1);
    strcpy(w->text, text);
    w->count = 1;
    w->next = head;
    head = w;
}

void remove_word(const char *text) {
    Word *prev = NULL;
    Word *w = head;
    while (w) {
        if (strcmp(w->text, text) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
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

int compare_words(const void *a, const void *b) {
    Word *wa = *(Word**)a;
    Word *wb = *(Word**)b;
    if (wb->count != wa->count) return wb->count - wa->count;
    return strcmp(wa->text, wb->text);
}

void top_words(void) {
    int cnt = 0;
    Word *w = head;
    while (w) { cnt++; w = w->next; }
    if (cnt == 0) return;
    Word **arr = malloc(cnt * sizeof(Word*));
    int i = 0;
    w = head;
    while (w) { arr[i++] = w; w = w->next; }
    qsort(arr, cnt, sizeof(Word*), compare_words);
    for (i = 0; i < cnt; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char word[4096];
    while (fscanf(f, "%4095s", word) == 1) {
        increment_word(word);
    }
    fclose(f);
}

void free_all(void) {
    Word *w = head;
    while (w) {
        Word *next = w->next;
        free(w->text);
        free(w);
        w = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
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
    return 0;
}