// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD 100

typedef struct {
    char word[MAX_WORD];
    int count;
} WordEntry;

WordEntry words[10000];
int word_count = 0;

int find_word(const char *word) {
    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i].word, word) == 0) return i;
    }
    return -1;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char line[MAX_WORD];
    while (fgets(line, sizeof(line), f)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        
        int idx = find_word(line);
        if (idx == -1) {
            strcpy(words[word_count].word, line);
            words[word_count].count = 1;
            word_count++;
        } else {
            words[idx].count++;
        }
    }
    fclose(f);
}

int compare_words(const void *a, const void *b) {
    WordEntry *wa = (WordEntry *)a;
    WordEntry *wb = (WordEntry *)b;
    if (wb->count != wa->count) return wb->count - wa->count;
    return strcmp(wa->word, wb->word);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char cmd[20], text[MAX_WORD];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "WORD") == 0) {
            fgets(text, MAX_WORD, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_word(text);
            if (idx == -1) {
                strcpy(words[word_count].word, text);
                words[word_count].count = 1;
                word_count++;
            } else {
                words[idx].count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            fgets(text, MAX_WORD, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_word(text);
            if (idx != -1) {
                for (int i = idx; i < word_count - 1; i++) {
                    words[i] = words[i + 1];
                }
                word_count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            fgets(text, MAX_WORD, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_word(text);
            if (idx != -1) {
                printf("%d\n", words[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(words, word_count, sizeof(WordEntry), compare_words);
            for (int i = 0; i < word_count; i++) {
                printf("%s %d\n", words[i].word, words[i].count);
            }
        }
    }
    
    return 0;
}