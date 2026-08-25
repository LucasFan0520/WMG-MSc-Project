// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WordCount {
    char *word;
    int count;
    struct WordCount *next;
} WordCount;

WordCount *head = NULL;

WordCount *find_word(char *word) {
    for (WordCount *w = head; w; w = w->next) {
        if (strcmp(w->word, word) == 0) return w;
    }
    return NULL;
}

void add_word(char *word) {
    WordCount *w = find_word(word);
    if (!w) {
        w = malloc(sizeof(WordCount));
        w->word = strdup(word);
        w->count = 0;
        w->next = head;
        head = w;
    }
    w->count++;
}

void remove_word(char *word) {
    WordCount *prev = NULL;
    WordCount *curr = head;
    while (curr) {
        if (strcmp(curr->word, word) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->word);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_count(char *word) {
    WordCount *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

void print_top() {
    for (WordCount *i = head; i; i = i->next) {
        for (WordCount *j = i->next; j; j = j->next) {
            if (i->count < j->count) {
                int temp_c = i->count;
                i->count = j->count;
                j->count = temp_c;
                char *temp_s = i->word;
                i->word = j->word;
                j->word = temp_s;
            }
        }
    }
    for (WordCount *w = head; w; w = w->next) {
        printf("%s %d\n", w->word, w->count);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (file) {
            char line[1000];
            while (fgets(line, sizeof(line), file)) {
                line[strcspn(line, "\n")] = 0;
                if (strlen(line) > 0) add_word(line);
            }
            fclose(file);
        }
    }
    char cmd[20], word[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "WORD") == 0) {
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            add_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            print_count(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            print_top();
        }
    }
    return 0;
}