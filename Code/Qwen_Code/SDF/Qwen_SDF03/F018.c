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

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Word *find_word(const char *text) {
    for (Word *w = head; w; w = w->next)
        if (strcmp(w->text, text) == 0) return w;
    return NULL;
}

int cmp(const void *a, const void *b) {
    const Word *x = *(const Word **)a, *y = *(const Word **)b;
    if (x->count != y->count) return y->count - x->count;
    return strcmp(x->text, y->text);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line) > 0) {
                    Word *w = find_word(line);
                    if (!w) {
                        w = malloc(sizeof(Word));
                        if (w) { w->text = strdup(line); w->count = 1; w->next = head; head = w; }
                    } else w->count++;
                }
            }
            fclose(f);
        }
    }
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "WORD") == 0) {
            Word *w = find_word(ptr);
            if (!w) {
                w = malloc(sizeof(Word));
                if (w) { w->text = strdup(ptr); w->count = 1; w->next = head; head = w; }
            } else w->count++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            Word *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->text, ptr) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->text); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            Word *w = find_word(ptr);
            printf("%d\n", w ? w->count : 0);
        } else if (strcmp(cmd, "TOP") == 0) {
            size_t count = 0;
            for (Word *w = head; w; w = w->next) count++;
            Word **arr = malloc(count * sizeof(Word *));
            if (arr) {
                size_t i = 0;
                for (Word *w = head; w; w = w->next) arr[i++] = w;
                qsort(arr, count, sizeof(Word *), cmp);
                for (i = 0; i < count; i++) printf("%s %d\n", arr[i]->text, arr[i]->count);
                free(arr);
            }
        }
    }
    while (head) {
        Word *next = head->next;
        free(head->text); free(head);
        head = next;
    }
    return 0;
}