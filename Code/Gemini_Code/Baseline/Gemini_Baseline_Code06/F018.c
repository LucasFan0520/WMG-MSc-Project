// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_freq(const void *a, const void *b) {
    WordFreq *wfA = (WordFreq *)a;
    WordFreq *wfB = (WordFreq *)b;
    return wfB->count - wfA->count;
}

int main(int argc, char *argv[]) {
    WordFreq *list = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                if (strlen(line) > 0) {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].word, line) == 0) {
                            list[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        list = realloc(list, (count + 1) * sizeof(WordFreq));
                        list[count].word = strdup(line);
                        list[count].count = 1;
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
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *text = line + 5;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    list[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                list = realloc(list, (count + 1) * sizeof(WordFreq));
                list[count].word = strdup(text);
                list[count].count = 1;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *text = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    free(list[i].word);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    list = realloc(list, count * sizeof(WordFreq));
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *text = line + 6;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    printf("%d\n", list[i].count);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("0\n");
        } else if (strcmp(line, "TOP") == 0) {
            WordFreq *temp = malloc(count * sizeof(WordFreq));
            memcpy(temp, list, count * sizeof(WordFreq));
            qsort(temp, count, sizeof(WordFreq), compare_freq);
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", temp[i].word, temp[i].count);
            }
            free(temp);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    return 0;
}
