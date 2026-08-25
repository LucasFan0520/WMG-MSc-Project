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

WordCount *find_word(const char *word) {
    WordCount *w = head;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *word) {
    WordCount *w = find_word(word);
    if (w) {
        w->count++;
        return;
    }
    WordCount *nw = malloc(sizeof(WordCount));
    if (!nw) return;
    nw->word = strdup(word);
    nw->count = 1;
    nw->next = NULL;
    if (!nw->word) { free(nw); return; }
    if (!head) head = nw;
    else {
        WordCount *last = head;
        while (last->next) last = last->next;
        last->next = nw;
    }
}

void remove_word(const char *word) {
    WordCount *w = head, *prev = NULL;
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
    WordCount *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

void top_words(void) {
    // simple insertion sort by count descending
    WordCount *sorted = NULL;
    WordCount *w = head;
    while (w) {
        WordCount *next = w->next;
        if (!sorted || w->count > sorted->count) {
            w->next = sorted;
            sorted = w;
        } else {
            WordCount *cur = sorted;
            while (cur->next && w->count <= cur->next->count)
                cur = cur->next;
            w->next = cur->next;
            cur->next = w;
        }
        w = next;
    }
    // print sorted
    WordCount *p = sorted;
    while (p) {
        printf("%s %d\n", p->word, p->count);
        p = p->next;
    }
}

void free_all(void) {
    WordCount *w = head;
    while (w) {
        WordCount *next = w->next;
        free(w->word);
        free(w);
        w = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line) > 0)
                    increment_word(line);
            }
            free(line);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], word[256];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            if (sscanf(line, "%*s %255s", word) == 1)
                increment_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", word) == 1)
                remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", word) == 1)
                count_word(word);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free(line);
    free_all();
    return 0;
}