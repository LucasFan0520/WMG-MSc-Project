// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

char *read_line_from_file(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *read_line() {
    return read_line_from_file(stdin);
}

int compare_freqs(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    if (wfB->count != wfA->count) {
        return wfB->count - wfA->count;
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *arr = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line_from_file(f);
                if (!line) break;
                if (strlen(line) > 0) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].word, line) == 0) {
                            arr[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        arr = realloc(arr, (count + 1) * sizeof(WordFreq));
                        arr[count].word = strdup(line);
                        arr[count].count = 1;
                        count++;
                    }
                }
                free(line);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *text = line + 5;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].word, text) == 0) {
                    arr[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                arr = realloc(arr, (count + 1) * sizeof(WordFreq));
                arr[count].word = strdup(text);
                arr[count].count = 1;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *text = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].word, text) == 0) {
                    free(arr[i].word);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *text = line + 6;
            int cval = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].word, text) == 0) {
                    cval = arr[i].count;
                    break;
                }
            }
            printf("%d\n", cval);
        } else if (strcmp(line, "TOP") == 0) {
            WordFreq *temp = malloc(count * sizeof(WordFreq));
            memcpy(temp, arr, count * sizeof(WordFreq));
            qsort(temp, count, sizeof(WordFreq), compare_freqs);
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", temp[i].word, temp[i].count);
            }
            free(temp);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].word);
    }
    free(arr);
    return 0;
}
