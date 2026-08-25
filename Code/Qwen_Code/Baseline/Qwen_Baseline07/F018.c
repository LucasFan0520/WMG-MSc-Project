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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        size_t l = strlen(line);
        if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
        char *token = strtok(line, " \t");
        while (token) {
            Word *w = head;
            while (w && strcmp(w->text, token) != 0) w = w->next;
            if (w) w->count++;
            else {
                Word *nw = malloc(sizeof(Word));
                nw->text = strdup(token); nw->count = 1; nw->next = head; head = nw;
            }
            token = strtok(NULL, " \t");
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char cmd[20], text[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "WORD") == 0) {
            scanf("%s", text);
            Word *w = head;
            while (w && strcmp(w->text, text) != 0) w = w->next;
            if (w) w->count++;
            else {
                Word *nw = malloc(sizeof(Word));
                nw->text = strdup(text); nw->count = 1; nw->next = head; head = nw;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", text);
            Word *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->text, text) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->text); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", text);
            for (Word *w = head; w; w = w->next)
                if (strcmp(w->text, text) == 0) { printf("%d\n", w->count); break; }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = 0;
            for (Word *w = head; w; w = w->next) n++;
            Word **arr = malloc(n * sizeof(Word*));
            Word *w = head;
            for (int i = 0; i < n; i++) { arr[i] = w; w = w->next; }
            for (int i = 0; i < n - 1; i++)
                for (int j = 0; j < n - i - 1; j++)
                    if (arr[j]->count < arr[j+1]->count) {
                        Word *t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t;
                    }
            for (int i = 0; i < n; i++) printf("%s %d\n", arr[i]->text, arr[i]->count);
            free(arr);
        }
    }
    return 0;
}