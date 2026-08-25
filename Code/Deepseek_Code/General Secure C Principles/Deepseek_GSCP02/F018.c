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
        w = malloc(sizeof(Word));
        if (!w) return;
        w->text = strdup(text);
        w->count = 1;
        w->next = head;
        head = w;
    }
}

void remove_word(const char *text) {
    Word *prev = NULL;
    Word *w = head;
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

int cmp_word(const void *p1, const void *p2) {
    Word *w1 = *(Word **)p1;
    Word *w2 = *(Word **)p2;
    return w2->count - w1->count; // descending
}

void top_words(void) {
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
    qsort(arr, n, sizeof(Word *), cmp_word);
    for (int i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void free_all(void) {
    while (head) {
        Word *next = head->next;
        free(head->text);
        free(head);
        head = next;
    }
}

int main(int argc, char *argv[]) {
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

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "WORD") == 0) {
            char text[1024];
            if (sscanf(line, "%*s %1023[^\n]", text) == 1) {
                increment_word(text);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char text[1024];
            if (sscanf(line, "%*s %1023[^\n]", text) == 1) {
                remove_word(text);
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char text[1024];
            if (sscanf(line, "%*s %1023[^\n]", text) == 1) {
                count_word(text);
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }

    free(line);
    free_all();
    return 0;
}