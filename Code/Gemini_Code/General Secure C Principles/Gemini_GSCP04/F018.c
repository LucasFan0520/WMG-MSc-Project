// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *word;
    long long count;
} WordFreq;

int compare_word_freq(const void *a, const void *b) {
    const WordFreq *wfA = (const WordFreq *)a;
    const WordFreq *wfB = (const WordFreq *)b;
    if (wfA->count != wfB->count) {
        return (wfB->count > wfA->count) - (wfB->count < wfA->count);
    }
    return strcmp(wfA->word, wfB->word);
}

int main(int argc, char **argv) {
    WordFreq *entries = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *fline = read_line(f);
                if (!fline) break;
                char *w = fline;
                while (*w == ' ' || *w == '\t') w++;
                size_t l = strlen(w);
                while (l > 0 && (w[l-1] == ' ' || w[l-1] == '\t')) {
                    w[l-1] = '\0';
                    l--;
                }
                if (*w != '\0') {
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(entries[i].word, w) == 0) {
                            entries[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            WordFreq *new_entries = realloc(entries, capacity * sizeof(WordFreq));
                            if (new_entries) entries = new_entries;
                        }
                        if (count < capacity) {
                            entries[count].word = strdup(w);
                            entries[count].count = 1;
                            count++;
                        }
                    }
                }
                free(fline);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*w != '\0') {
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(entries[i].word, w) == 0) {
                        entries[i].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        WordFreq *new_entries = realloc(entries, capacity * sizeof(WordFreq));
                        if (new_entries) entries = new_entries;
                    }
                    if (count < capacity) {
                        entries[count].word = strdup(w);
                        entries[count].count = 1;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].word, w) == 0) {
                    free(entries[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        entries[j] = entries[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *w = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].word, w) == 0) {
                    cnt = entries[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (count > 0) {
                qsort(entries, count, sizeof(WordFreq), compare_word_freq);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", entries[i].word, entries[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(entries[i].word);
    }
    free(entries);
    return 0;
}
