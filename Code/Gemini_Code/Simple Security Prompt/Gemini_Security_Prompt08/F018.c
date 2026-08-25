// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
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

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_wf(const void *a, const void *b) {
    WordFreq *wa = (WordFreq *)a;
    WordFreq *wb = (WordFreq *)b;
    if (wb->count != wa->count) {
        return wb->count - wa->count;
    }
    return strcmp(wa->word, wb->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                if (strlen(line) > 0) {
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(list[i].word, line) == 0) {
                            list[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            WordFreq *nb = realloc(list, cap * sizeof(WordFreq));
                            if (nb) list = nb;
                        }
                        if (count < cap) {
                            list[count].word = strdup(line);
                            list[count].count = 1;
                            count++;
                        }
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
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "WORD") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, arg1) == 0) {
                    list[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    WordFreq *nb = realloc(list, cap * sizeof(WordFreq));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].word = strdup(arg1);
                    list[count].count = 1;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, arg1) == 0) {
                    free(list[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int c = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, arg1) == 0) {
                    c = list[i].count;
                    break;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(list, count, sizeof(WordFreq), compare_wf);
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", list[i].word, list[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    return 0;
}
