// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct W {
    char *word;
    int count;
    struct W *next;
} W;

W *head = NULL;

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            W *p = head;
            while (p && strcmp(p->word, line)) {
                p = p->next;
            }
            if (p) {
                p->count++;
            } else {
                W *nw = malloc(sizeof(W));
                nw->word = strdup(line);
                nw->count = 1;
                nw->next = head;
                head = nw;
            }
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char word[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "WORD")) {
            fgets(word, 1000, stdin);
            word[strcspn(word, "\n")] = 0;
            if (word[0] == ' ') {
                memmove(word, word + 1, strlen(word));
            }
            W *p = head;
            while (p && strcmp(p->word, word)) {
                p = p->next;
            }
            if (p) {
                p->count++;
            } else {
                W *nw = malloc(sizeof(W));
                nw->word = strdup(word);
                nw->count = 1;
                nw->next = head;
                head = nw;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            fgets(word, 1000, stdin);
            word[strcspn(word, "\n")] = 0;
            if (word[0] == ' ') {
                memmove(word, word + 1, strlen(word));
            }
            W *p = head;
            W *prev = NULL;
            while (p) {
                if (!strcmp(p->word, word)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->word);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            fgets(word, 1000, stdin);
            word[strcspn(word, "\n")] = 0;
            if (word[0] == ' ') {
                memmove(word, word + 1, strlen(word));
            }
            W *p = head;
            while (p && strcmp(p->word, word)) {
                p = p->next;
            }
            printf("%d\n", p ? p->count : 0);
        } else if (!strcmp(cmd, "TOP")) {
            W *arr[1000];
            int n = 0;
            for (W *p = head; p; p = p->next) {
                arr[n++] = p;
            }
            for (int i = 0; i < n - 1; i++) {
                for (int j = 0; j < n - i - 1; j++) {
                    if (arr[j]->count < arr[j+1]->count) {
                        W *tmp = arr[j];
                        arr[j] = arr[j+1];
                        arr[j+1] = tmp;
                    }
                }
            }
            for (int i = 0; i < n; i++) {
                printf("%s %d\n", arr[i]->word, arr[i]->count);
            }
        }
    }
    return 0;
}