// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *word;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

Word *find_word(const char *w) {
    Word *p = words;
    while (p) {
        if (strcmp(p->word, w) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_word(const char *w) {
    Word *p = find_word(w);
    if (p) {
        p->count++;
    } else {
        p = malloc(sizeof(Word));
        p->word = strdup(w);
        p->count = 1;
        p->next = words;
        words = p;
    }
}

void remove_word(const char *w) {
    Word *prev = NULL;
    Word *p = words;
    while (p) {
        if (strcmp(p->word, w) == 0) {
            if (prev) prev->next = p->next;
            else words = p->next;
            free(p->word);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void count_word(const char *w) {
    Word *p = find_word(w);
    printf("%d\n", p ? p->count : 0);
}

int compare_count(const void *a, const void *b) {
    Word *wa = *(Word **)a;
    Word *wb = *(Word **)b;
    return wb->count - wa->count;
}

void top_words() {
    int n = 0;
    Word *p = words;
    while (p) { n++; p = p->next; }
    if (n == 0) return;
    Word **arr = malloc(n * sizeof(Word *));
    p = words;
    int i = 0;
    while (p) { arr[i++] = p; p = p->next; }
    qsort(arr, n, sizeof(Word *), compare_count);
    for (i = 0; i < n; i++) {
        printf("%s:%d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

void free_all() {
    Word *p = words;
    while (p) {
        Word *next = p->next;
        free(p->word);
        free(p);
        p = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                add_word(line);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "WORD") == 0) {
            add_word(rest);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_word(rest);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_word(rest);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}