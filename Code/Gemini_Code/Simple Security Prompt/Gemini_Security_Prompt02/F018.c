// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *word;
    int count;
} WordFreq;

int cmp_freq(const void *a, const void *b) {
    WordFreq *pa = (WordFreq *)a;
    WordFreq *pb = (WordFreq *)b;
    if (pb->count != pa->count) {
        return pb->count - pa->count;
    }
    return strcmp(pa->word, pb->word);
}

int main(int argc, char **argv) {
    WordFreq *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                if (strlen(line) > 0) {
                    int found = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].word, line) == 0) {
                            found = (int)i;
                            break;
                        }
                    }
                    if (found != -1) {
                        arr[found].count++;
                    } else {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            arr = realloc(arr, cap * sizeof(WordFreq));
                        }
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
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *w = line + 5;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                arr[found].count++;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(WordFreq));
                }
                arr[count].word = strdup(w);
                arr[count].count = 1;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *w = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    free(arr[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *w = line + 6;
            int cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, w) == 0) {
                    cnt = arr[i].count;
                    break;
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(line, "TOP") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(WordFreq), cmp_freq);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", arr[i].word, arr[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].word);
    }
    free(arr);
    return 0;
}
