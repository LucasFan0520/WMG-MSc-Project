// F018.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    const WordFreq *wfA = (const WordFreq *)a;
    const WordFreq *wfB = (const WordFreq *)b;
    if (wfA->count > wfB->count) return -1;
    if (wfA->count < wfB->count) return 1;
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while ((read = getline(&line, &line_len, fp)) != -1) {
                if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
                if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
                if (strlen(line) == 0) continue;
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].word, line) == 0) {
                        list[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    WordFreq *temp = realloc(list, (count + 1) * sizeof(WordFreq));
                    if (temp) {
                        list = temp;
                        list[count].word = strdup(line);
                        list[count].count = 1;
                        count++;
                    }
                }
            }
            fclose(fp);
        }
    }
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    list[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                WordFreq *temp = realloc(list, (count + 1) * sizeof(WordFreq));
                if (temp) {
                    list = temp;
                    list[count].word = strdup(w);
                    list[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    free(list[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            int c = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    c = list[i].count;
                    break;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(list, count, sizeof(WordFreq), compare_freq);
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", list[i].word, list[i].count);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    free(line);
    return 0;
}
