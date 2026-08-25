// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    long long count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    if (wfA->count != wfB->count) {
        return (wfB->count > wfA->count) - (wfB->count < wfA->count);
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while ((read = getline(&line, &cap, fp)) != -1) {
                line[strcspn(line, "\r\n")] = 0;
                char *tok = strtok(line, " ");
                while (tok) {
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].word, tok) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        list[found].count++;
                    } else {
                        WordFreq *tmp = realloc(list, (count + 1) * sizeof(WordFreq));
                        if (tmp) {
                            list = tmp;
                            list[count].word = strdup(tok);
                            list[count].count = 1;
                            count++;
                        }
                    }
                    tok = strtok(NULL, " ");
                }
            }
            fclose(fp);
        }
    }
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *text = line + 5;
            while (*text == ' ') {
                text++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                list[found].count++;
            } else {
                WordFreq *tmp = realloc(list, (count + 1) * sizeof(WordFreq));
                if (tmp) {
                    list = tmp;
                    list[count].word = strdup(text);
                    list[count].count = 1;
                    count++;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *text = line + 7;
            while (*text == ' ') {
                text++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    free(list[i].word);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *text = line + 6;
            while (*text == ' ') {
                text++;
            }
            long long ans = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    ans = list[i].count;
                    break;
                }
            }
            printf("%lld\n", ans);
        } else if (strcmp(line, "TOP") == 0) {
            qsort(list, count, sizeof(WordFreq), compare_freq);
            for (int i = 0; i < count; i++) {
                printf("%s %lld\n", list[i].word, list[i].count);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    return 0;
}
