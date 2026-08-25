// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    long count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    const WordFreq *wfA = (const WordFreq *)a;
    const WordFreq *wfB = (const WordFreq *)b;
    if (wfA->count < wfB->count) return 1;
    if (wfA->count > wfB->count) return -1;
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *items = NULL;
    size_t count = 0;
    size_t capacity = 0;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            long fread_bytes;
            while ((fread_bytes = getline(&fline, &flen, f)) != -1) {
                if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                    fline[fread_bytes - 1] = '\0';
                    fread_bytes--;
                }
                if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                    fline[fread_bytes - 1] = '\0';
                    fread_bytes--;
                }

                char *w = fline;
                while (*w == ' ' || *w == '	') w++;
                if (*w == '\0') continue;

                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(items[i].word, w) == 0) {
                        found = (int)i;
                        break;
                    }
                }

                if (found != -1) {
                    items[found].count++;
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        WordFreq *new_items = realloc(items, capacity * sizeof(WordFreq));
                        if (!new_items) exit(1);
                        items = new_items;
                    }
                    items[count].word = strdup(w);
                    if (!items[count].word) exit(1);
                    items[count].count = 1;
                    count++;
                }
            }
            free(fline);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "WORD") == 0) {
            char *w = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, w) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                items[found].count++;
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *new_items = realloc(items, capacity * sizeof(WordFreq));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].word = strdup(w);
                if (!items[count].word) exit(1);
                items[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *w = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, w) == 0) {
                    free(items[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *w = p;
            long cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, w) == 0) {
                    cval = items[i].count;
                    break;
                }
            }
            printf("%ld\n", cval);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(items, count, sizeof(WordFreq), compare_freq);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %ld\n", items[i].word, items[i].count);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].word);
    }
    free(items);
    free(line);
    return 0;
}
