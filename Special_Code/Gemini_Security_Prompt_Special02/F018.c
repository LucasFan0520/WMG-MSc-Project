// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    if (wfB->count != wfA->count) return wfB->count - wfA->count;
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                if (*line) {
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(items[i].word, line) == 0) {
                            items[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            items = realloc(items, cap * sizeof(WordFreq));
                        }
                        items[count].word = mystrdup(line);
                        items[count].count = 1;
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
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *text = p;
        while (*p && *p != ' ') p++;
        if (*p) *p = '\0';
        if (strcmp(cmd, "WORD") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, text) == 0) {
                    items[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(WordFreq));
                }
                items[count].word = mystrdup(text);
                items[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, text) == 0) {
                    free(items[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, text) == 0) {
                    printf("%d\n", items[i].count);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("0\n");
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(items, count, sizeof(WordFreq), compare_freq);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", items[i].word, items[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].word);
    }
    free(items);
    return 0;
}
