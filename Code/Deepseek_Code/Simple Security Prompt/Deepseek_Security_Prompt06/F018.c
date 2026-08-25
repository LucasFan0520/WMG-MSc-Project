// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *head = NULL;

Word *find_word(const char *text) {
    Word *w = head;
    while (w) {
        if (strcmp(w->text, text) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
    } else {
        Word *new = malloc(sizeof(Word));
        new->text = malloc(strlen(text) + 1);
        strcpy(new->text, text);
        new->count = 1;
        new->next = head;
        head = new;
    }
}

void remove_word(const char *text) {
    Word *w = head, *prev = NULL;
    while (w) {
        if (strcmp(w->text, text) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
            free(w->text);
            free(w);
            return;
        }
        prev = w;
        w = w->next;
    }
}

void count_word(const char *text) {
    Word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

int cmp_desc(const void *a, const void *b) {
    Word *const *pa = a;
    Word *const *pb = b;
    return (*pb)->count - (*pa)->count;
}

void top_words(void) {
    int n = 0;
    Word *w = head;
    while (w) { n++; w = w->next; }
    if (n == 0) return;
    Word **arr = malloc(n * sizeof(Word *));
    w = head;
    for (int i = 0; i < n; i++) { arr[i] = w; w = w->next; }
    qsort(arr, n, sizeof(Word *), cmp_desc);
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void load_words(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (!fp) return;
    char word[4096];
    while (fscanf(fp, "%4095s", word) == 1) {
        increment_word(word);
    }
    fclose(fp);
}

void free_all(void) {
    Word *w = head;
    while (w) {
        Word *tmp = w->next;
        free(w->text);
        free(w);
        w = tmp;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_words(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char text[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "WORD") == 0) {
            if (*rest) increment_word(rest);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*rest) remove_word(rest);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (*rest) count_word(rest);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}