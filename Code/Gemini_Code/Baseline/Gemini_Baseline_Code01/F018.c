// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    if (wfB->count != wfA->count) {
        return wfB->count - wfA->count;
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    int count = 0;
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char fline[256];
            while (fgets(fline, sizeof(fline), fp)) {
                fline[strcspn(fline, "\r\n")] = 0;
                if (strlen(fline) == 0) continue;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].word, fline) == 0) {
                        list[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    list = realloc(list, (count + 1) * sizeof(WordFreq));
                    list[count].word = strdup(fline);
                    list[count].count = 1;
                    count++;
                }
            }
            fclose(fp);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "WORD") == 0) {
            if (strlen(args) > 0) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].word, args) == 0) {
                        list[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    list = realloc(list, (count + 1) * sizeof(WordFreq));
                    list[count].word = strdup(args);
                    list[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, args) == 0) {
                    free(list[i].word);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    list = realloc(list, count * sizeof(WordFreq));
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int c = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, args) == 0) {
                    c = list[i].count;
                    break;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(list, count, sizeof(WordFreq), compare_freq);
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", list[i].word, list[i].count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    return 0;
}
