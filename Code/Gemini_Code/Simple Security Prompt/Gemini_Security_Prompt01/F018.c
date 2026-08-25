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
    WordFreq *table = NULL;
    int count = 0;

    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, fp)) != -1) {
                fline[strcspn(fline, "\r\n")] = '\0';
                if (strlen(fline) == 0) continue;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].word, fline) == 0) {
                        table[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    table = realloc(table, (count + 1) * sizeof(WordFreq));
                    if (!table) return 1;
                    table[count].word = strdup(fline);
                    table[count].count = 1;
                    count++;
                }
            }
            free(fline);
            fclose(fp);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "WORD") == 0 && args) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, args) == 0) {
                    table[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                table = realloc(table, (count + 1) * sizeof(WordFreq));
                if (!table) return 1;
                table[count].word = strdup(args);
                table[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, args) == 0) {
                    free(table[i].word);
                    for (int j = i; j < count - 1; j++) {
                        table[j] = table[j + 1];
                    }
                    count--;
                    table = realloc(table, count * sizeof(WordFreq));
                    if (count > 0 && !table) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0 && args) {
            int c = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].word, args) == 0) {
                    c = table[i].count;
                    break;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(table, count, sizeof(WordFreq), compare_freq);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", table[i].word, table[i].count);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(table[i].word);
    }
    free(table);
    return 0;
}
