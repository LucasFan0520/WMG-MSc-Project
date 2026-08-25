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
    Word *w = words;
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
        if (!new) return;
        new->text = strdup(text);
        new->count = 1;
        new->next = words;
        words = new;
    }
}

void remove_word(const char *text) {
    Word *prev = NULL, *cur = words;
    while (cur) {
        if (strcmp(cur->text, text) == 0) {
            if (prev) prev->next = cur->next;
            else words = cur->next;
            free(cur->text);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void count_word(const char *text) {
    Word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

int compare_word(const void *a, const void *b) {
    Word *wa = *(Word **)a;
    Word *wb = *(Word **)b;
    return wb->count - wa->count; // descending
}

void top_words(void) {
    int count = 0;
    Word *cur = words;
    while (cur) { count++; cur = cur->next; }
    if (count == 0) return;
    Word **arr = malloc(count * sizeof(Word *));
    if (!arr) return;
    cur = words;
    for (int i = 0; i < count; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    qsort(arr, count, sizeof(Word *), compare_word);
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void free_all(void) {
    Word *w = words;
    while (w) {
        Word *next = w->next;
        free(w->text);
        free(w);
        w = next;
    }
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
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            increment_word(p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *text = p;
            char *space = strchr(text, ' ');
            if (space) *space = '\0';
            remove_word(text);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *text = p;
            char *space = strchr(text, ' ');
            if (space) *space = '\0';
            count_word(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}