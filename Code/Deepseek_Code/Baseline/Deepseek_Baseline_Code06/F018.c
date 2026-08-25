/* F018.c */
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

void increment_word(const char *word) {
    Word *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(Word));
        w->word = strdup(word);
        w->count = 1;
        w->next = head;
        head = w;
    }
}

void remove_word(const char *word) {
    Word *w = head, *prev = NULL;
    while (w) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
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

void top_words() {
    // Simple insertion sort on linked list by count descending
    Word *sorted = NULL;
    Word *w = head;
    while (w) {
        Word *next = w->next;
        if (!sorted || w->count > sorted->count) {
            w->next = sorted;
            sorted = w;
        } else {
            Word *cur = sorted;
            while (cur->next && cur->next->count >= w->count) cur = cur->next;
            w->next = cur->next;
            cur->next = w;
        }
        w = next;
    }
    head = sorted;
    // print
    Word *p = head;
    while (p) {
        printf("%s %d\n", p->word, p->count);
        p = p->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        increment_word(line);
    }
    fclose(f);
}

void free_all() {
    while (head) {
        Word *tmp = head;
        head = head->next;
        free(tmp->word);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char word[4096];
        int n = sscanf(line, "%31s %[^\n]", cmd, word);
        if (strcmp(cmd, "WORD") == 0 && n == 2) {
            increment_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0 && n == 2) {
            count_word(word);
        } else if (strcmp(cmd, "TOP") == 0 && n == 1) {
            top_words();
        }
    }
    free_all();
    return 0;
}