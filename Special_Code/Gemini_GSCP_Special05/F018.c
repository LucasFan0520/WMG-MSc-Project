// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *word;
    long long count;
} WordFreq;

int compare_freqs(const void *a, const void *b) {
    const WordFreq *aa = (const WordFreq *)a;
    const WordFreq *bb = (const WordFreq *)b;
    if (aa->count < bb->count) return 1;
    if (aa->count > bb->count) return -1;
    return strcmp(aa->word, bb->word);
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                if (strlen(fline) > 0) {
                    size_t found = (size_t)-1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(list[i].word, fline) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != (size_t)-1) {
                        list[found].count++;
                    } else {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            WordFreq *nb = realloc(list, cap * sizeof(WordFreq));
                            if (!nb) {
                                free(fline);
                                break;
                            }
                            list = nb;
                        }
                        list[count].word = safe_dup(fline);
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
        if (strncmp(line, "WORD ", 5) == 0) {
            char *text = line + 5;
            size_t found = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != (size_t)-1) {
                list[found].count++;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    WordFreq *nb = realloc(list, cap * sizeof(WordFreq));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    list = nb;
                }
                list[count].word = safe_dup(text);
                list[count].count = 1;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *text = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    free(list[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *text = line + 6;
            long long current = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    current = list[i].count;
                    break;
                }
            }
            printf("%lld\n", current);
        } else if (strcmp(line, "TOP") == 0) {
            qsort(list, count, sizeof(WordFreq), compare_freqs);
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", list[i].word, list[i].count);
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
