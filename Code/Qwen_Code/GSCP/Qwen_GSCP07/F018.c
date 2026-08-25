// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word { char *text; int count; struct Word *next; } Word;
Word *head = NULL;

void add_word(const char *text) {
    for (Word *w = head; w; w = w->next) {
        if (strcmp(w->text, text) == 0) { w->count++; return; }
    }
    Word *w = malloc(sizeof(Word));
    w->text = strdup(text);
    w->count = 1;
    w->next = head;
    head = w;
}

void remove_word(const char *text) {
    Word **p = &head;
    while (*p) {
        if (strcmp((*p)->text, text) == 0) {
            Word *tmp = *p;
            *p = tmp->next;
            free(tmp->text);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void count_word(const char *text) {
    for (Word *w = head; w; w = w->next) {
        if (strcmp(w->text, text) == 0) { printf("%d\n", w->count); return; }
    }
    printf("0\n");
}

int cmp_word(const void *a, const void *b) {
    Word *wa = *(Word **)a, *wb = *(Word **)b;
    return wb->count - wa->count;
}

void top_words() {
    int n = 0;
    for (Word *w = head; w; w = w->next) n++;
    if (n == 0) return;
    Word **arr = malloc(n * sizeof(Word *));
    Word *p = head;
    for (int i = 0; i < n; i++) { arr[i] = p; p = p->next; }
    qsort(arr, n, sizeof(Word *), cmp_word);
    for (int i = 0; i < n; i++) printf("%s %d\n", arr[i]->text, arr[i]->count);
    free(arr);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) add_word(line);
    }
    fclose(f);
}

void cleanup() {
    while (head) {
        Word *tmp = head->next;
        free(head->text);
        free(head);
        head = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *t = strchr(line, ' ');
            if (t) add_word(t + 1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *t = strchr(line, ' ');
            if (t) remove_word(t + 1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *t = strchr(line, ' ');
            if (t) count_word(t + 1);
        } else if (strcmp(cmd, "TOP") == 0) top_words();
    }
    cleanup();
    return 0;
}