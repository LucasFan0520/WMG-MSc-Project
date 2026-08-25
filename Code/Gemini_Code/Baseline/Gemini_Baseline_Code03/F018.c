// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

int main(int argc, char *argv[]) {
    WordFreq *list = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[256];
            while (fgets(fline, sizeof(fline), f)) {
                int flen = strlen(fline);
                while (flen > 0 && (fline[flen - 1] == '\n' || fline[flen - 1] == '\r')) {
                    fline[flen - 1] = '\0';
                    flen--;
                }
                if (flen > 0) {
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].word, fline) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        list[found].count++;
                    } else {
                        list = realloc(list, (count + 1) * sizeof(WordFreq));
                        list[count].word = strdup(fline);
                        list[count].count = 1;
                        count++;
                    }
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "WORD") == 0) {
            char text[256];
            if (sscanf(ptr, "%255s", text) > 0) {
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
                    list = realloc(list, (count + 1) * sizeof(WordFreq));
                    list[count].word = strdup(text);
                    list[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char text[256];
            if (sscanf(ptr, "%255s", text) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].word, text) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].word);
                    for (int i = found; i < count - 1; i++) {
                        list[i] = list[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        list = realloc(list, count * sizeof(WordFreq));
                    } else {
                        free(list);
                        list = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char text[256];
            if (sscanf(ptr, "%255s", text) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].word, text) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%d\n", list[found].count);
                } else {
                    printf("0\n");
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            for (int i = 0; i < count - 1; i++) {
                for (int j = 0; j < count - i - 1; j++) {
                    if (list[j].count < list[j + 1].count) {
                        WordFreq temp = list[j];
                        list[j] = list[j + 1];
                        list[j + 1] = temp;
                    }
                }
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
