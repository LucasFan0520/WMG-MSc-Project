// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *word;
    int count;
} WordFreq;

int compare_words(const void *a, const void *b) {
    const WordFreq *aa = (const WordFreq *)a;
    const WordFreq *bb = (const WordFreq *)b;
    if (aa->count > bb->count) return -1;
    if (aa->count < bb->count) return 1;
    return strcmp(aa->word, bb->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                char *w = fline;
                while (*w == ' ') w++;
                size_t wl = strlen(w);
                while (wl > 0 && w[wl - 1] == ' ') {
                    w[wl - 1] = '\0';
                    wl--;
                }
                if (wl > 0) {
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(list[i].word, w) == 0) {
                            list[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            WordFreq *new_list = realloc(list, capacity * sizeof(WordFreq));
                            if (new_list) list = new_list;
                        }
                        list[count].word = mystrdup(w);
                        list[count].count = 1;
                        count++;
                    }
                }
                free(fline);
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
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "WORD") == 0) {
            char *w = p;
            while (*w == ' ') w++;
            char *w_end = w;
            while (*w_end && *w_end != ' ') w_end++;
            *w_end = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    list[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *new_list = realloc(list, capacity * sizeof(WordFreq));
                    if (new_list) list = new_list;
                }
                list[count].word = mystrdup(w);
                list[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *w = p;
            while (*w == ' ') w++;
            char *w_end = w;
            while (*w_end && *w_end != ' ') w_end++;
            *w_end = '\0';
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
            char *w = p;
            while (*w == ' ') w++;
            char *w_end = w;
            while (*w_end && *w_end != ' ') w_end++;
            *w_end = '\0';
            int c = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    c = list[i].count;
                    break;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(list, count, sizeof(WordFreq), compare_words);
            }
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
