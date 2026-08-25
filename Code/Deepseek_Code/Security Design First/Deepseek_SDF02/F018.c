/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct wf {
    char *word;
    int count;
    struct wf *next;
} wf_t;

wf_t *head = NULL;

wf_t *find_word(const char *word) {
    wf_t *w = head;
    while (w) {
        if (strcmp(w->word, word) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void inc_word(const char *word) {
    wf_t *w = find_word(word);
    if (w) {
        w->count++;
        return;
    }
    w = malloc(sizeof(wf_t));
    if (!w) return;
    w->word = strdup(word);
    w->count = 1;
    w->next = head;
    head = w;
    if (!w->word) { free(w); }
}

void remove_word(const char *word) {
    wf_t *prev = NULL, *w = head;
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
    wf_t *w = find_word(word);
    printf("%d\n", w ? w->count : 0);
}

int cmp_desc(const void *a, const void *b) {
    wf_t *wa = *(wf_t**)a;
    wf_t *wb = *(wf_t**)b;
    return wb->count - wa->count;
}

void top_words(void) {
    int n = 0;
    wf_t *w = head;
    while (w) { n++; w = w->next; }
    if (n == 0) return;
    wf_t **arr = malloc(sizeof(wf_t*) * n);
    if (!arr) return;
    int i = 0;
    w = head;
    while (w) { arr[i++] = w; w = w->next; }
    qsort(arr, n, sizeof(wf_t*), cmp_desc);
    for (i = 0; i < n; i++)
        printf("%s %d\n", arr[i]->word, arr[i]->count);
    free(arr);
}

void free_all(void) {
    wf_t *w = head;
    while (w) {
        wf_t *nxt = w->next;
        free(w->word);
        free(w);
        w = nxt;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = 0;
                inc_word(line);
            }
            free(line);
            fclose(f);
        }
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], word[256];
        if (sscanf(line, "%9s %255s", cmd, word) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "WORD") == 0) {
                inc_word(rest);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                remove_word(rest);
            } else if (strcmp(cmd, "COUNT") == 0) {
                count_word(rest);
            } else if (strcmp(cmd, "TOP") == 0) {
                top_words();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}