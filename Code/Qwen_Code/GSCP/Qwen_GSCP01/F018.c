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

Word *find_word(const char *text) {
    Word *curr = words;
    while (curr) {
        if (strcmp(curr->text, text) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void increment_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(Word));
        if (!w) exit(EXIT_FAILURE);
        w->text = strdup(text);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(const char *text) {
    Word *curr = words;
    Word *prev = NULL;
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
    if (w) printf("%d\n", w->count);
    else printf("0\n");
}

int cmp_words(const void *a, const void *b) {
    const Word *wa = *(const Word **)a;
    const Word *wb = *(const Word **)b;
    if (wa->count != wb->count) return (wb->count - wa->count);
    return strcmp(wa->text, wb->text);
}

void top_words() {
    int count = 0;
    Word *curr = words;
    while (curr) {
        count++;
        curr = curr->next;
    }
    if (count == 0) return;
    
    Word **arr = malloc(count * sizeof(Word *));
    if (!arr) exit(EXIT_FAILURE);
    
    curr = words;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    
    qsort(arr, count, sizeof(Word *), cmp_words);
    
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
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

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            increment_word(line);
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char text[3800] = {0};
        
        int matched = sscanf(line, "%31s %[^\n]", cmd, text);
        
        if (strcmp(cmd, "WORD") == 0 && matched == 2) {
            increment_word(text);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_word(text);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_word(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}