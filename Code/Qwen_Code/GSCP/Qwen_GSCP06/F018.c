// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Word {
    char* text;
    int count;
    struct Word* next;
} Word;

Word* head = NULL;

Word* find_word(const char* text) {
    for (Word* w = head; w; w = w->next) {
        if (strcmp(w->text, text) == 0) return w;
    }
    return NULL;
}

int cmp_word(const void* a, const void* b) {
    Word* A = *(Word**)a;
    Word* B = *(Word**)b;
    return B->count - A->count;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if (f) {
            char l[MAX_LINE];
            while (fgets(l, sizeof(l), f)) {
                l[strcspn(l, "\n")] = 0;
                Word* w = find_word(l);
                if (w) w->count++;
                else {
                    Word* n = malloc(sizeof(Word));
                    n->text = strdup(l);
                    n->count = 1;
                    n->next = head;
                    head = n;
                }
            }
            fclose(f);
        }
    }
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "WORD") == 0) {
            Word* w = find_word(p);
            if (w) w->count++;
            else {
                Word* n = malloc(sizeof(Word));
                n->text = strdup(p);
                n->count = 1;
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            Word* prev = NULL;
            for (Word* w = head; w; prev = w, w = w->next) {
                if (strcmp(w->text, p) == 0) {
                    if (prev) prev->next = w->next;
                    else head = w->next;
                    free(w->text);
                    free(w);
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            Word* w = find_word(p);
            printf("%d\n", w ? w->count : 0);
        } else if (strcmp(cmd, "TOP") == 0) {
            int count = 0;
            for (Word* w = head; w; w = w->next) count++;
            Word** arr = malloc(count * sizeof(Word*));
            Word* w = head;
            for (int i = 0; i < count; i++) {
                arr[i] = w;
                w = w->next;
            }
            qsort(arr, count, sizeof(Word*), cmp_word);
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", arr[i]->text, arr[i]->count);
            }
            free(arr);
        }
    }
    return 0;
}