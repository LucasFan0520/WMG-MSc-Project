// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *word;
    int count;
    struct Word *next;
} Word;

Word *head = NULL;
Word *tail = NULL;

Word *find_word(const char *word) {
    Word *w = head;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *word) {
    Word *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        Word *new = malloc(sizeof(Word));
        new->word = malloc(strlen(word)+1);
        strcpy(new->word, word);
        new->count = 1;
        new->next = NULL;
        if (tail) tail->next = new;
        else head = new;
        tail = new;
    }
}

void remove_word(const char *word) {
    Word *prev = NULL;
    Word *w = head;
    while (w) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
            if (w == tail) tail = prev;
            free(w->word);
            free(w);
            return;
        }
        prev = w;
        w = w->next;
    }
}

void count_word(const char *word) {
    Word *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

int compare_word(const void *a, const void *b) {
    Word *wa = *(Word**)a;
    Word *wb = *(Word**)b;
    return wb->count - wa->count; // descending
}

void top_words(void) {
    if (!head) return;
    int n = 0;
    Word *w = head;
    while (w) { n++; w = w->next; }
    Word **arr = malloc(n * sizeof(Word*));
    w = head;
    for (int i=0; i<n; i++) { arr[i] = w; w = w->next; }
    qsort(arr, n, sizeof(Word*), compare_word);
    for (int i=0; i<n; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

void free_all(void) {
    Word *w = head;
    while (w) {
        Word *next = w->next;
        free(w->word);
        free(w);
        w = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                increment_word(line);
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "WORD") == 0) {
            char *word = strtok(rest, " ");
            if (word) increment_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *word = strtok(rest, " ");
            if (word) remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *word = strtok(rest, " ");
            if (word) count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}