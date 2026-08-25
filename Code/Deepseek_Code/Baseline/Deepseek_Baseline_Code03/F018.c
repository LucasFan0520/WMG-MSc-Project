// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct word {
    char *text;
    int count;
    struct word *next;
};

struct word *words = NULL;

struct word *find_word(char *text) {
    struct word *w = words;
    while (w) {
        if (strcmp(w->text, text) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(char *text) {
    struct word *w = find_word(text);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(struct word));
        w->text = malloc(strlen(text) + 1);
        strcpy(w->text, text);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(char *text) {
    struct word *w = words, *prev = NULL;
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

void count_word(char *text) {
    struct word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

int compare_words(const void *a, const void *b) {
    struct word *wa = *(struct word **)a;
    struct word *wb = *(struct word **)b;
    return wb->count - wa->count; // descending
}

void top_words() {
    int count = 0;
    struct word *w = words;
    while (w) { count++; w = w->next; }
    if (count == 0) return;
    struct word **arr = malloc(count * sizeof(struct word *));
    w = words;
    for (int i = 0; i < count; i++) {
        arr[i] = w;
        w = w->next;
    }
    qsort(arr, count, sizeof(struct word *), compare_words);
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                increment_word(line);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char text[4096];
        int n = sscanf(line, "%31s %4095[^\n]", cmd, text);
        if (strcmp(cmd, "WORD") == 0 && n >= 2) {
            increment_word(text);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_word(text);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_word(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    return 0;
}