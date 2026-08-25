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
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            Word *p = malloc(sizeof(Word));
            p->text = strdup(line);
            p->count = 1;
            p->next = head;
            head = p;
        }
    }
    fclose(f);
}

void word_inc(const char *text) {
    for (Word *p = head; p; p = p->next) {
        if (strcmp(p->text, text) == 0) {
            p->count++;
            return;
        }
    }
    Word *p = malloc(sizeof(Word));
    p->text = strdup(text);
    p->count = 1;
    p->next = head;
    head = p;
}

void word_remove(const char *text) {
    Word *p = head;
    Word *prev = NULL;
    while (p) {
        if (strcmp(p->text, text) == 0) {
            Word *next = p->next;
            free(p->text);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void word_count(const char *text) {
    for (Word *p = head; p; p = p->next) {
        if (strcmp(p->text, text) == 0) {
            printf("%d\n", p->count);
            return;
        }
    }
    printf("0\n");
}

int cmp_word(const void *a, const void *b) {
    Word *x = *(Word **)a;
    Word *y = *(Word **)b;
    return y->count - x->count;
}

void word_top() {
    int n = 0;
    for (Word *p = head; p; p = p->next) {
        n++;
    }
    Word **arr = malloc(n * sizeof(Word *));
    int i = 0;
    for (Word *p = head; p; p = p->next) {
        arr[i++] = p;
    }
    qsort(arr, n, sizeof(Word *), cmp_word);
    for (i = 0; i < n; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[1000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char text[800];
        text[0] = 0;
        sscanf(line, "%s %[^\n]", cmd, text);
        if (strcmp(cmd, "WORD") == 0) {
            word_inc(text);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            word_remove(text);
        } else if (strcmp(cmd, "COUNT") == 0) {
            word_count(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            word_top();
        }
    }
    while (head) {
        Word *n = head->next;
        free(head->text);
        free(head);
        head = n;
    }
    return 0;
}