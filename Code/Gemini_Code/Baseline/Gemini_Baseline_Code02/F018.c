// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *word;
    int count;
} WFreq;

int compare_wfreq(const void *a, const void *b) {
    WFreq *wa = (WFreq *)a;
    WFreq *wb = (WFreq *)b;
    return (wb->count - wa->count);
}

int main(int argc, char **argv) {
    WFreq *freqs = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(freqs[i].word, line) == 0) {
                        freqs[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    freqs = realloc(freqs, sizeof(WFreq) * (count + 1));
                    freqs[count].word = strdup(line);
                    freqs[count].count = 1;
                    count++;
                }
                free(line);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "TOP") == 0) {
                if (count > 0) {
                    WFreq *sorted = malloc(sizeof(WFreq) * count);
                    memcpy(sorted, freqs, sizeof(WFreq) * count);
                    qsort(sorted, count, sizeof(WFreq), compare_wfreq);
                    for (int i = 0; i < count; i++) {
                        printf("%s %d\n", sorted[i].word, sorted[i].count);
                    }
                    free(sorted);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *text = p1 + 1;
        if (strcmp(cmd, "WORD") == 0) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, text) == 0) {
                    freqs[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                freqs = realloc(freqs, sizeof(WFreq) * (count + 1));
                freqs[count].word = strdup(text);
                freqs[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, text) == 0) {
                    free(freqs[i].word);
                    for (int j = i; j < count - 1; j++) {
                        freqs[j] = freqs[j + 1];
                    }
                    count--;
                    if (count == 0) { free(freqs); freqs = NULL; }
                    else { freqs = realloc(freqs, sizeof(WFreq) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int c = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, text) == 0) {
                    c = freqs[i].count;
                    break;
                }
            }
            printf("%d\n", c);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(freqs[i].word);
    }
    free(freqs);
    return 0;
}
