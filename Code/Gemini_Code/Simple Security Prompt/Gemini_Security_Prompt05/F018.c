// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_wf(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    if (wfB->count != wfA->count) return wfB->count - wfA->count;
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *arr = NULL;
    int count = 0;
    int cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                char *p = line;
                char *w = get_token(&p);
                if (w) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].word, w) == 0) {
                            arr[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            WordFreq *narr = realloc(arr, cap * sizeof(WordFreq));
                            if (narr) arr = narr;
                        }
                        arr[count].word = strdup(w);
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
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            char *w = get_rest(&p);
            if (strlen(w) > 0) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].word, w) == 0) {
                        arr[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        WordFreq *narr = realloc(arr, cap * sizeof(WordFreq));
                        if (narr) arr = narr;
                    }
                    arr[count].word = strdup(w);
                    arr[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *w = get_rest(&p);
            if (strlen(w) > 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].word, w) == 0) {
                        free(arr[i].word);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *w = get_rest(&p);
            int cnt = 0;
            if (strlen(w) > 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].word, w) == 0) {
                        cnt = arr[i].count;
                        break;
                    }
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(arr, count, sizeof(WordFreq), compare_wf);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", arr[i].word, arr[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].word);
    }
    free(arr);
    return 0;
}
