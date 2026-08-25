// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void increment_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(Word));
        if (!w) return;
        w->text = safe_strdup(text);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(const char *text) {
    Word *prev = NULL, *cur = words;
    while (cur) {
        if (strcmp(cur->text, text) == 0) {
            if (prev) prev->next = cur->next;
            else words = cur->next;
            free(cur->text);
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

int compare_word(const void *a, const void *b) {
    Word *wa = *(Word **)a;
    Word *wb = *(Word **)b;
    if (wa->count != wb->count) return wb->count - wa->count;
    return strcmp(wa->text, wb->text);
}

void top_words(void) {
    int n = 0;
    Word *cur = words;
    while (cur) { n++; cur = cur->next; }
    if (n == 0) return;
    Word **arr = malloc(n * sizeof(Word *));
    if (!arr) return;
    cur = words;
    for (int i = 0; i < n; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    qsort(arr, n, sizeof(Word *), compare_word);
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        increment_word(line);
    }
    fclose(f);
}

void free_all(void) {
    Word *w = words;
    while (w) {
        Word *next = w->next;
        free(w->text);
        free(w);
        w = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) load_file(argv[1]);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "WORD") == 0 && n >= 2) {
            increment_word(arg1);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_word(arg1);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_word(arg1);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}