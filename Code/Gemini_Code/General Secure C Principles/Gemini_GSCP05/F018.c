// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    const WordFreq *wfA = (const WordFreq *)a;
    const WordFreq *wfB = (const WordFreq *)b;
    if (wfA->count != wfB->count) {
        return wfB->count - wfA->count;
    }
    return strcmp(wfA->word, wfB->word);
}

void add_word(WordFreq **wf, int *count, int *capacity, const char *w) {
    for (int i = 0; i < *count; i++) {
        if (strcmp((*wf)[i].word, w) == 0) {
            (*wf)[i].count++;
            return;
        }
    }
    if (*count >= *capacity) {
        *capacity = *capacity == 0 ? 4 : *capacity * 2;
        WordFreq *new_wf = realloc(*wf, *capacity * sizeof(WordFreq));
        if (new_wf) *wf = new_wf;
    }
    if (*count < *capacity) {
        (*wf)[*count].word = strdup(w);
        (*wf)[*count].count = 1;
        (*count)++;
    }
}

int main(int argc, char **argv) {
    WordFreq *wf = NULL;
    int count = 0;
    int capacity = 0;
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                if (strlen(line) > 0) {
                    add_word(&wf, &count, &capacity, line);
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *text = line + 5;
            while (*text == ' ') text++;
            add_word(&wf, &count, &capacity, text);
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *text = line + 7;
            while (*text == ' ') text++;
            for (int i = 0; i < count; i++) {
                if (strcmp(wf[i].word, text) == 0) {
                    free(wf[i].word);
                    for (int j = i; j < count - 1; j++) {
                        wf[j] = wf[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *text = line + 6;
            while (*text == ' ') text++;
            int res = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(wf[i].word, text) == 0) {
                    res = wf[i].count;
                    break;
                }
            }
            printf("%d\n", res);
        } else if (strcmp(line, "TOP") == 0) {
            if (count > 0) {
                qsort(wf, count, sizeof(WordFreq), compare_freq);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", wf[i].word, wf[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(wf[i].word);
    }
    free(wf);
    return 0;
}
