// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *word;
    int count;
} Freq;

typedef struct {
    Freq *items;
    size_t count;
    size_t capacity;
} WordFreq;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void wf_init(WordFreq *wf) {
    wf->items = NULL;
    wf->count = 0;
    wf->capacity = 0;
}

static void wf_free(WordFreq *wf) {
    for (size_t i = 0; i < wf->count; i++)
        free(wf->items[i].word);
    free(wf->items);
    wf->items = NULL;
    wf->count = 0;
    wf->capacity = 0;
}

static int wf_find(WordFreq *wf, const char *word) {
    for (size_t i = 0; i < wf->count; i++) {
        if (strcmp(wf->items[i].word, word) == 0)
            return (int)i;
    }
    return -1;
}

static void wf_word(WordFreq *wf, const char *word) {
    int idx = wf_find(wf, word);
    if (idx >= 0) {
        wf->items[idx].count++;
        return;
    }
    if (wf->count == wf->capacity) {
        size_t newcap = wf->capacity ? wf->capacity * 2 : 4;
        Freq *tmp = realloc(wf->items, newcap * sizeof(Freq));
        if (!tmp) return;
        wf->items = tmp;
        wf->capacity = newcap;
    }
    wf->items[wf->count].word = safe_strdup(word);
    wf->items[wf->count].count = 1;
    if (wf->items[wf->count].word)
        wf->count++;
    else
        free(wf->items[wf->count].word);
}

static void wf_remove(WordFreq *wf, const char *word) {
    int idx = wf_find(wf, word);
    if (idx < 0) return;
    free(wf->items[idx].word);
    for (size_t i = idx; i < wf->count - 1; i++)
        wf->items[i] = wf->items[i+1];
    wf->count--;
}

static void wf_count(WordFreq *wf, const char *word) {
    int idx = wf_find(wf, word);
    printf("%d\n", idx >= 0 ? wf->items[idx].count : 0);
}

static int cmp_desc(const void *a, const void *b) {
    const Freq *pa = (const Freq *)a;
    const Freq *pb = (const Freq *)b;
    return pb->count - pa->count;
}

static void wf_top(WordFreq *wf) {
    if (wf->count == 0) return;
    Freq *copy = malloc(wf->count * sizeof(Freq));
    if (!copy) return;
    for (size_t i = 0; i < wf->count; i++) {
        copy[i].word = safe_strdup(wf->items[i].word);
        copy[i].count = wf->items[i].count;
        if (!copy[i].word) {
            for (size_t j = 0; j < i; j++) free(copy[j].word);
            free(copy);
            return;
        }
    }
    qsort(copy, wf->count, sizeof(Freq), cmp_desc);
    for (size_t i = 0; i < wf->count; i++) {
        printf("%s %d\n", copy[i].word, copy[i].count);
        free(copy[i].word);
    }
    free(copy);
}

static void load_file(WordFreq *wf, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) > 0)
            wf_word(wf, line);
    }
    fclose(f);
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
}

int main(int argc, char **argv) {
    WordFreq wf;
    wf_init(&wf);
    if (argc > 1) {
        load_file(&wf, argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1));
        if (strcmp(cmd, "WORD") == 0) {
            wf_word(&wf, a1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            wf_remove(&wf, a1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            wf_count(&wf, a1);
        } else if (strcmp(cmd, "TOP") == 0) {
            wf_top(&wf);
        }
    }
    wf_free(&wf);
    return 0;
}