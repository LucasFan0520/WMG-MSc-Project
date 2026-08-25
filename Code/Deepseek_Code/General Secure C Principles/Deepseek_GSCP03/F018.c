/* F018.c */
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
        return;
    }
    Word *new_w = malloc(sizeof(Word));
    if (!new_w) return;
    new_w->text = strdup(text);
    new_w->count = 1;
    new_w->next = head;
    head = new_w;
}

void remove_word(const char *text) {
    Word *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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

void top_words(void) {
    // collect into array and sort
    int n = 0;
    Word *w = head;
    while (w) { n++; w = w->next; }
    if (n == 0) return;
    Word **arr = malloc(n * sizeof(Word *));
    if (!arr) return;
    w = head;
    for (int i = 0; i < n; i++) {
        arr[i] = w;
        w = w->next;
    }
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i]->count < arr[j]->count || 
                (arr[i]->count == arr[j]->count && strcmp(arr[i]->text, arr[j]->text) > 0)) {
                Word *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void free_all(void) {
    Word *w = head;
    while (w) {
        Word *tmp = w;
        w = w->next;
        free(tmp->text);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                increment_word(line);
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            increment_word(p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_word(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            count_word(p);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}