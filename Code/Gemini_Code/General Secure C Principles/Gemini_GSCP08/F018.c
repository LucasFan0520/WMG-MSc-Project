// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *word;
    long count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    const WordFreq *aa = (const WordFreq *)a;
    const WordFreq *bb = (const WordFreq *)b;
    if (aa->count > bb->count) return -1;
    if (aa->count < bb->count) return 1;
    return strcmp(aa->word, bb->word);
}

int main(int argc, char **argv) {
    WordFreq *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                size_t wl = strlen(fline);
                while (wl > 0 && (fline[wl - 1] == '\n' || fline[wl - 1] == '\r')) {
                    fline[wl - 1] = '\0';
                    wl--;
                }
                if (wl > 0) {
                    int found = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].word, fline) == 0) {
                            found = (int)i;
                            break;
                        }
                    }
                    if (found != -1) {
                        arr[found].count++;
                    } else {
                        if (count >= cap) {
                            size_t ncap = cap == 0 ? 4 : cap * 2;
                            WordFreq *narr = realloc(arr, ncap * sizeof(WordFreq));
                            if (narr) {
                                arr = narr;
                                cap = ncap;
                            }
                        }
                        if (count < cap) {
                            arr[count].word = strdup(fline);
                            arr[count].count = 1;
                            count++;
                        }
                    }
                }
                free(fline);
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
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "TOP") == 0) {
                if (count > 0) {
                    qsort(arr, count, sizeof(WordFreq), compare_freq);
                    for (size_t i = 0; i < count; i++) {
                        printf("%s %ld\n", arr[i].word, arr[i].count);
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *text = p1 + 1;
        if (strcmp(cmd, "WORD") == 0) {
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, text) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                arr[found].count++;
            } else {
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    WordFreq *narr = realloc(arr, ncap * sizeof(WordFreq));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].word = strdup(text);
                    arr[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, text) == 0) {
                    free(arr[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            long val = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].word, text) == 0) {
                    val = arr[i].count;
                    break;
                }
            }
            printf("%ld\n", val);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].word);
    }
    free(arr);
    return 0;
}
