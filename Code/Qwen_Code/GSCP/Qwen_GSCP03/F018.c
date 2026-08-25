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

void free_list() {
    while (head) {
        Word *t = head; head = head->next;
        free(t->text); free(t);
    }
}

Word *find_word(const char *text) {
    for (Word *w = head; w; w = w->next)
        if (strcmp(w->text, text) == 0) return w;
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                Word *w = find_word(line);
                if (w) w->count++;
                else {
                    w = malloc(sizeof(Word));
                    w->text = strdup(line); w->count = 1; w->next = head; head = w;
                }
            }
            fclose(f);
        }
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], text[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "WORD") == 0) {
            if (sscanf(line, "%*s %255[^\n]", text) != 1) continue;
            Word *w = find_word(text);
            if (w) w->count++;
            else {
                w = malloc(sizeof(Word));
                w->text = strdup(text); w->count = 1; w->next = head; head = w;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255[^\n]", text) != 1) continue;
            Word *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->text, text) == 0) {
                    Word *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->text); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255[^\n]", text) != 1) continue;
            Word *w = find_word(text);
            printf("%d\n", w ? w->count : 0);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = 0;
            for (Word *w = head; w; w = w->next) n++;
            Word **arr = malloc(n * sizeof(Word*));
            int i = 0;
            for (Word *w = head; w; w = w->next) arr[i++] = w;
            for (int a = 0; a < n - 1; a++) {
                for (int b = a + 1; b < n; b++) {
                    if (arr[a]->count < arr[b]->count) {
                        Word *tmp = arr[a]; arr[a] = arr[b]; arr[b] = tmp;
                    }
                }
            }
            for (int a = 0; a < n; a++) printf("%s %d\n", arr[a]->text, arr[a]->count);
            free(arr);
        }
    }
    free_list();
    return 0;
}