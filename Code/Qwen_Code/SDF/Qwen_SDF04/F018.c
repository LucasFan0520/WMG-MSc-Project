// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* word;
    int count;
} WordFreq;

WordFreq* words = NULL;
size_t w_count = 0;
size_t w_cap = 0;

void cleanup() {
    for (size_t i = 0; i < w_count; i++) {
        free(words[i].word);
    }
    free(words);
}

int cmp_freq(const void* a, const void* b) {
    const WordFreq* ia = (const WordFreq*)a;
    const WordFreq* ib = (const WordFreq*)b;
    return ib->count - ia->count;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if (f) {
            char* line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
                if (strlen(line) > 0) {
                    int found = 0;
                    for (size_t i = 0; i < w_count; i++) {
                        if (strcmp(words[i].word, line) == 0) {
                            words[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (w_count == w_cap) {
                            size_t new_cap = w_cap == 0 ? 4 : w_cap * 2;
                            WordFreq* temp = realloc(words, new_cap * sizeof(WordFreq));
                            if (!temp) { free(line); fclose(f); return 1; }
                            words = temp;
                            w_cap = new_cap;
                        }
                        words[w_count].word = strdup(line);
                        words[w_count].count = 1;
                        if (!words[w_count].word) { free(line); fclose(f); return 1; }
                        w_count++;
                    }
                }
            }
            fclose(f);
            free(line);
        }
    }
    
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
        
        if (strcmp(cmd, "WORD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* word = ptr;
            int found = 0;
            for (size_t i = 0; i < w_count; i++) {
                if (strcmp(words[i].word, word) == 0) {
                    words[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (w_count == w_cap) {
                    size_t new_cap = w_cap == 0 ? 4 : w_cap * 2;
                    WordFreq* temp = realloc(words, new_cap * sizeof(WordFreq));
                    if (!temp) { cleanup(); free(line); return 1; }
                    words = temp;
                    w_cap = new_cap;
                }
                words[w_count].word = strdup(word);
                words[w_count].count = 1;
                if (!words[w_count].word) { cleanup(); free(line); return 1; }
                w_count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* word = ptr;
            for (size_t i = 0; i < w_count; i++) {
                if (strcmp(words[i].word, word) == 0) {
                    free(words[i].word);
                    for (size_t j = i; j < w_count - 1; j++) {
                        words[j] = words[j + 1];
                    }
                    w_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* word = ptr;
            int count = 0;
            for (size_t i = 0; i < w_count; i++) {
                if (strcmp(words[i].word, word) == 0) {
                    count = words[i].count;
                    break;
                }
            }
            printf("%d\n", count);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (w_count > 0) {
                qsort(words, w_count, sizeof(WordFreq), cmp_freq);
            }
            for (size_t i = 0; i < w_count; i++) {
                printf("%s %d\n", words[i].word, words[i].count);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}