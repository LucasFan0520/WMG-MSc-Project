// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line_file(FILE *f) {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *word; int count; } Word;
Word *words = NULL;
size_t wc = 0, wcap = 0;

void add_word(const char *w) {
    for (size_t i = 0; i < wc; i++) {
        if (strcmp(words[i].word, w) == 0) { words[i].count++; return; }
    }
    if (wc == wcap) {
        size_t nc = wcap ? wcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Word)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Word *tmp = realloc(words, nc * sizeof(Word));
        if (!tmp) { perror("realloc"); exit(1); }
        words = tmp;
        wcap = nc;
    }
    words[wc].word = strdup(w);
    words[wc].count = 1;
    if (!words[wc].word) { perror("strdup"); exit(1); }
    wc++;
}

void rem_word(const char *w) {
    for (size_t i = 0; i < wc; i++) {
        if (strcmp(words[i].word, w) == 0) {
            free(words[i].word);
            words[i] = words[--wc];
            return;
        }
    }
}

int cmp_word(const void *a, const void *b) {
    const Word *wa = a, *wb = b;
    return wb->count - wa->count;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line_file(f)) != NULL) {
        add_word(line);
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "WORD") == 0) {
            char *text = strchr(line, ' ');
            if (text) text++;
            if (text) add_word(text);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *text = strchr(line, ' ');
            if (text) text++;
            if (text) rem_word(text);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *text = strchr(line, ' ');
            if (text) text++;
            if (text) {
                for (size_t i = 0; i < wc; i++) {
                    if (strcmp(words[i].word, text) == 0) { printf("%d\n", words[i].count); break; }
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(words, wc, sizeof(Word), cmp_word);
            for (size_t i = 0; i < wc; i++) printf("%s %d\n", words[i].word, words[i].count);
        }
        free(line);
    }
    for (size_t i = 0; i < wc; i++) free(words[i].word);
    free(words);
    return 0;
}