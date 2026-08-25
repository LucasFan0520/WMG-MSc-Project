/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WordFreq {
    char *word;
    int count;
    struct WordFreq *next;
} WordFreq;

WordFreq *head = NULL;

WordFreq *find_word(char *word) {
    for (WordFreq *w = head; w; w = w->next)
        if (strcmp(w->word, word) == 0) return w;
    return NULL;
}

void increment_word(char *word) {
    WordFreq *w = find_word(word);
    if (w) w->count++;
    else {
        WordFreq *new = malloc(sizeof(WordFreq));
        new->word = strdup(word);
        new->count = 1;
        new->next = head;
        head = new;
    }
}

void remove_word(char *word) {
    WordFreq *prev = NULL;
    for (WordFreq *w = head; w; w = w->next) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
            free(w->word);
            free(w);
            return;
        }
        prev = w;
    }
}

void count_word(char *word) {
    WordFreq *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

void top_all(void) {
    // simple selection sort? we can use list and sort by count descending.
    // We'll copy to array for simplicity (since list not huge).
    int n = 0;
    for (WordFreq *w = head; w; w = w->next) n++;
    if (n == 0) return;
    WordFreq **arr = malloc(n * sizeof(WordFreq*));
    int i = 0;
    for (WordFreq *w = head; w; w = w->next) arr[i++] = w;
    for (int j = 0; j < n-1; j++) {
        for (int k = j+1; k < n; k++) {
            if (arr[k]->count > arr[j]->count) {
                WordFreq *tmp = arr[j];
                arr[j] = arr[k];
                arr[k] = tmp;
            }
        }
    }
    for (int j = 0; j < n; j++)
        printf("%s %d\n", arr[j]->word, arr[j]->count);
    free(arr);
}

void free_all(void) {
    WordFreq *w = head;
    while (w) {
        WordFreq *next = w->next;
        free(w->word);
        free(w);
        w = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        return 1;
    }
    if (argc == 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = '\0';
                increment_word(line);
            }
            free(line);
            fclose(f);
        } else {
            perror("fopen");
            return 1;
        }
    }

    // stdin commands
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256];
        int n = sscanf(line, "%15s %[^\n]", cmd, arg1);
        if (n < 1) continue;
        if (strcmp(cmd, "WORD") == 0 && n == 2) {
            increment_word(arg1);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_word(arg1);
        } else if (strcmp(cmd, "COUNT") == 0 && n == 2) {
            count_word(arg1);
        } else if (strcmp(cmd, "TOP") == 0 && n == 1) {
            top_all();
        }
    }
    free(line);
    free_all();
    return 0;
}