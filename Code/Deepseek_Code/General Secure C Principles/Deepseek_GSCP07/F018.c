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

Word* find_word(const char *word) {
    Word *w = head;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void free_word(Word *w) {
    free(w->word);
    free(w);
}

void increment_word(const char *word) {
    Word *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        Word *new = malloc(sizeof(Word));
        new->word = strdup(word);
        new->count = 1;
        new->next = NULL;
        if (!head) head = new;
        else {
            Word *last = head;
            while (last->next) last = last->next;
            last->next = new;
        }
    }
}

void remove_word(const char *word) {
    Word *w = head, *prev = NULL;
    while (w) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
            free_word(w);
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

void top() {
    // Bubble sort by count descending, then print
    int count = 0;
    Word *w = head;
    while (w) { count++; w = w->next; }
    if (count == 0) return;
    Word **arr = malloc(count * sizeof(Word*));
    w = head;
    for (int i = 0; i < count; i++) { arr[i] = w; w = w->next; }
    for (int i = 0; i < count-1; i++) {
        for (int j = i+1; j < count; j++) {
            if (arr[i]->count < arr[j]->count) {
                Word *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line) > 0) increment_word(line);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "WORD") == 0) {
            char *word = strtok(NULL, " ");
            if (word) increment_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *word = strtok(NULL, " ");
            if (word) remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *word = strtok(NULL, " ");
            if (word) count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            top();
        }
    }
    while (head) {
        Word *tmp = head;
        head = head->next;
        free_word(tmp);
    }
    return 0;
}