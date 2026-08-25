// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        Word *w = malloc(sizeof(Word));
        w->text = strdup(line);
        w->count = 1;
        w->next = words;
        words = w;
    }
    fclose(f);
}

Word *find_word(const char *text) {
    Word *curr = words;
    while (curr) {
        if (strcmp(curr->text, text) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_word(const char *text) {
    Word *w = find_word(text);
    if (w) w->count++;
    else {
        w = malloc(sizeof(Word));
        w->text = strdup(text);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(const char *text) {
    Word *curr = words, *prev = NULL;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            if (prev) prev->next = curr->next;
            else words = curr->next;
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_word(const char *text) {
    Word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

void top_words() {
    int n = 0;
    Word *curr = words;
    while (curr) { n++; curr = curr->next; }
    if (n == 0) return;
    
    Word **arr = malloc(n * sizeof(Word*));
    curr = words;
    for (int i = 0; i < n; i++) { arr[i] = curr; curr = curr->next; }
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j]->count < arr[j+1]->count) {
                Word *tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
    for (int i = 0; i < n; i++) printf("%s %d\n", arr[i]->text, arr[i]->count);
    free(arr);
}

void free_all() {
    Word *curr = words;
    while (curr) {
        Word *next = curr->next;
        free(curr->text);
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], text[4096] = {0};
        int n = sscanf(line, "%31s %4095[^\n]", cmd, text);
        
        if (strcmp(cmd, "WORD") == 0 && n >= 1) add_word(text);
        else if (strcmp(cmd, "REMOVE") == 0 && n >= 1) remove_word(text);
        else if (strcmp(cmd, "COUNT") == 0 && n >= 1) count_word(text);
        else if (strcmp(cmd, "TOP") == 0) top_words();
    }
    free_all();
    return 0;
}