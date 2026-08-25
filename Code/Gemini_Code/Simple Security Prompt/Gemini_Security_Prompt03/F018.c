// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

char *read_line(FILE *fp) {
    size_t size = 32;
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

int compare_freqs(const void *a, const void *b) {
    WordFreq *wA = (WordFreq *)a;
    WordFreq *wB = (WordFreq *)b;
    if (wA->count != wB->count) {
        return wB->count - wA->count;
    }
    return strcmp(wA->word, wB->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    int count = 0;
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char *p = line;
                while (*p == ' ') p++;
                if (*p != '\0') {
                    char *end = p;
                    while (*end && *end != ' ') end++;
                    if (*end) *end = '\0';
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (list[i].word && strcmp(list[i].word, p) == 0) {
                            list[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        list = realloc(list, (count + 1) * sizeof(WordFreq));
                        list[count].word = strdup(p);
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
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            while (*p == ' ') p++;
            char *w = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].word && strcmp(list[i].word, w) == 0) {
                    list[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                list = realloc(list, (count + 1) * sizeof(WordFreq));
                list[count].word = strdup(w);
                list[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *w = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].word && strcmp(list[i].word, w) == 0) {
                    free(list[i].word);
                    list[i].word = NULL;
                    list[i].count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *w = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int c_val = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].word && strcmp(list[i].word, w) == 0) {
                    c_val = list[i].count;
                    break;
                }
            }
            printf("%d\n", c_val);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(list, count, sizeof(WordFreq), compare_freqs);
                for (int i = 0; i < count; i++) {
                    if (list[i].word) {
                        printf("%s %d\n", list[i].word, list[i].count);
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].word) free(list[i].word);
    }
    free(list);
    return 0;
}
