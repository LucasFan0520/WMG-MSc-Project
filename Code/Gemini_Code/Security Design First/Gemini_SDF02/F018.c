/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_words(const void *a, const void *b) {
    WordFreq *wA = *(WordFreq **)a;
    WordFreq *wB = *(WordFreq **)b;
    if (wA->count > wB->count) return -1;
    if (wA->count < wB->count) return 1;
    return strcmp(wA->word, wB->word);
}

int main(int argc, char *argv[]) {
    WordFreq *freqs = NULL;
    size_t count = 0;
    size_t capacity = 0;

    char *line = NULL;
    size_t len = 0;

    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (getline(&line, &len, fp) != -1) {
                size_t str_len = strlen(line);
                while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
                    line[str_len - 1] = '\0';
                    str_len--;
                }
                char *word = line;
                while (*word == ' ') word++;
                if (*word == '\0') continue;

                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, word) == 0) {
                        freqs[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        WordFreq *temp = realloc(freqs, new_cap * sizeof(WordFreq));
                        if (!temp) break;
                        freqs = temp;
                        capacity = new_cap;
                    }
                    freqs[count].word = strdup(word);
                    freqs[count].count = 1;
                    if (freqs[count].word) {
                        count++;
                    }
                }
            }
            fclose(fp);
        }
    }

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;
        char *text = p;

        if (strcmp(cmd, "WORD") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, text) == 0) {
                    freqs[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *temp = realloc(freqs, new_cap * sizeof(WordFreq));
                    if (!temp) break;
                    freqs = temp;
                    capacity = new_cap;
                }
                freqs[count].word = strdup(text);
                freqs[count].count = 1;
                if (freqs[count].word) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, text) == 0) {
                    free(freqs[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        freqs[j] = freqs[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int c_val = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, text) == 0) {
                    c_val = freqs[i].count;
                    break;
                }
            }
            printf("%d\n", c_val);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                WordFreq **temp_arr = malloc(count * sizeof(WordFreq *));
                if (temp_arr) {
                    for (size_t i = 0; i < count; i++) {
                        temp_arr[i] = &freqs[i];
                    }
                    qsort(temp_arr, count, sizeof(WordFreq *), compare_words);
                    for (size_t i = 0; i < count; i++) {
                        printf("%s %d\n", temp_arr[i]->word, temp_arr[i]->count);
                    }
                    free(temp_arr);
                }
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(freqs[i].word);
    }
    free(freqs);
    return 0;
}
