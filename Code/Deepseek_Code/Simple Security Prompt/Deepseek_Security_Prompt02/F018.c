/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

Word *find_word(const char *text) {
    Word *w = words;
    while (w) {
        if (strcmp(w->text, text) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void free_word(Word *w) {
    free(w->text);
    free(w);
}

void word_increment(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
        return;
    }
    w = malloc(sizeof(Word));
    w->text = strdup(text);
    w->count = 1;
    w->next = words;
    words = w;
}

void word_remove(const char *text) {
    Word *prev = NULL, *curr = words;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            if (prev) prev->next = curr->next;
            else words = curr->next;
            free_word(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void word_count(const char *text) {
    Word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

int compare_words(const void *a, const void *b) {
    Word *A = *(Word **)a;
    Word *B = *(Word **)b;
    return B->count - A->count;
}

void top_words(void) {
    int count = 0;
    Word *w = words;
    while (w) { count++; w = w->next; }
    if (count == 0) return;
    Word **arr = malloc(count * sizeof(Word *));
    w = words;
    int i = 0;
    while (w) { arr[i++] = w; w = w->next; }
    qsort(arr, count, sizeof(Word *), compare_words);
    for (i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        if (strlen(line) > 0) word_increment(line);
    }
    fclose(f);
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);

    char line[4096];
    char cmd[32];
    char text[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            word_increment(p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            word_remove(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            word_count(p);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }

    while (words) {
        Word *tmp = words;
        words = words->next;
        free_word(tmp);
    }
    return 0;
}