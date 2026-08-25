// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int freq;
} WordFreq;

WordFreq *freq_table = NULL;
int count = 0;
int capacity = 0;

int find_word(const char *w) {
    for (int i = 0; i < count; i++) {
        if (strcmp(freq_table[i].word, w) == 0) {
            return i;
        }
    }
    return -1;
}

void add_word(const char *w) {
    int idx = find_word(w);
    if (idx != -1) {
        freq_table[idx].freq++;
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        freq_table = realloc(freq_table, capacity * sizeof(WordFreq));
    }
    freq_table[count].word = strdup(w);
    freq_table[count].freq = 1;
    count++;
}

void remove_word(const char *w) {
    int idx = find_word(w);
    if (idx != -1) {
        free(freq_table[idx].word);
        for (int i = idx; i < count - 1; i++) {
            freq_table[i] = freq_table[i + 1];
        }
        count--;
    }
}

void count_word(const char *w) {
    int idx = find_word(w);
    if (idx != -1) {
        printf("%d\n", freq_table[idx].freq);
    } else {
        printf("0\n");
    }
}

int compare_freq(const void *a, const void *b) {
    const WordFreq *wfA = (const WordFreq *)a;
    const WordFreq *wfB = (const WordFreq *)b;
    if (wfB->freq != wfA->freq) {
        return wfB->freq - wfA->freq;
    }
    return strcmp(wfA->word, wfB->word);
}

void print_top(void) {
    qsort(freq_table, count, sizeof(WordFreq), compare_freq);
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", freq_table[i].word, freq_table[i].freq);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *file_line = NULL;
            size_t file_len = 0;
            ssize_t file_read;
            while ((file_read = getline(&file_line, &file_len, fp)) != -1) {
                if (file_read > 0 && file_line[file_read - 1] == '\n') {
                    file_line[file_read - 1] = '\0';
                }
                if (strlen(file_line) > 0) {
                    add_word(file_line);
                }
            }
            free(file_line);
            fclose(fp);
        }
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *w = strtok(NULL, " ");
            if (w) add_word(w);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *w = strtok(NULL, " ");
            if (w) remove_word(w);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *w = strtok(NULL, " ");
            if (w) count_word(w);
        } else if (strcmp(cmd, "TOP") == 0) {
            print_top();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(freq_table[i].word);
    }
    free(freq_table);
    return 0;
}
