/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct wf {
    char *word;
    int count;
    struct wf *next;
} WF;

WF *head = NULL;

WF *find_word(const char *word) {
    WF *w = head;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void increment_word(const char *word) {
    WF *w = find_word(word);
    if (w) { w->count++; return; }
    WF *new = malloc(sizeof(WF));
    new->word = strdup(word);
    new->count = 1;
    new->next = head;
    head = new;
}

void remove_word(const char *word) {
    WF *w = head, *prev = NULL;
    while (w) {
        if (strcmp(w->word, word) == 0) {
            if (prev) prev->next = w->next;
            else head = w->next;
            free(w->word); free(w);
            return;
        }
        prev = w;
        w = w->next;
    }
}

void count_word(const char *word) {
    WF *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

int cmp_wf(const void *p1, const void *p2) {
    WF *a = *(WF **)p1, *b = *(WF **)p2;
    return b->count - a->count; // descending
}

void top() {
    int count = 0;
    WF *w = head;
    while (w) { count++; w = w->next; }
    if (!count) return;
    WF **arr = malloc(count * sizeof(WF *));
    w = head;
    for (int i = 0; i < count; i++) { arr[i] = w; w = w->next; }
    qsort(arr, count, sizeof(WF *), cmp_wf);
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char word[256];
            while (fgets(word, sizeof(word), f)) {
                word[strcspn(word, "\n")] = '\0';
                if (strlen(word) > 0) increment_word(word);
            }
            fclose(f);
        }
    }

    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], text[256];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TOP") == 0) { top(); continue; }
        if (strcmp(cmd, "WORD") == 0) {
            char *rest = line + strlen(cmd) + 1;
            if (*rest == ' ') rest++;
            increment_word(rest);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *rest = line + strlen(cmd) + 1;
            if (*rest == ' ') rest++;
            remove_word(rest);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *rest = line + strlen(cmd) + 1;
            if (*rest == ' ') rest++;
            count_word(rest);
        }
    }
    return 0;
}