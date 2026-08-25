// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    WordFreq *wA = (WordFreq *)a;
    WordFreq *wB = (WordFreq *)b;
    if (wB->count != wA->count) {
        return wB->count - wA->count;
    }
    return strcmp(wA->word, wB->word);
}

int main(int argc, char **argv) {
    WordFreq *freqs = NULL;
    int count = 0;
    int capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char w[256];
                if (sscanf(line, "%255s", w) == 1) {
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(freqs[i].word, w) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        freqs[found].count++;
                    } else {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            freqs = realloc(freqs, capacity * sizeof(WordFreq));
                        }
                        freqs[count].word = strdup(w);
                        freqs[count].count = 1;
                        count++;
                    }
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TOP") == 0) {
            WordFreq *temp = malloc(count * sizeof(WordFreq));
            memcpy(temp, freqs, count * sizeof(WordFreq));
            qsort(temp, count, sizeof(WordFreq), compare_freq);
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", temp[i].word, temp[i].count);
            }
            free(temp);
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        char w[256];
        if (sscanf(p, "%255s", w) == 1) {
            if (strcmp(cmd, "WORD") == 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, w) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    freqs[found].count++;
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        freqs = realloc(freqs, capacity * sizeof(WordFreq));
                    }
                    freqs[count].word = strdup(w);
                    freqs[count].count = 1;
                    count++;
                }
            } else if (strcmp(cmd, "REMOVE") == 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, w) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(freqs[found].word);
                    for (int i = found; i < count - 1; i++) {
                        freqs[i] = freqs[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "COUNT") == 0) {
                int amt = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, w) == 0) {
                        amt = freqs[i].count;
                        break;
                    }
                }
                printf("%d\n", amt);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(freqs[i].word);
    }
    free(freqs);
    return 0;
}
