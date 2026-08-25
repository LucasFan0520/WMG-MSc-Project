/* F018.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordEntry;

static WordEntry *words = NULL;
static int wcount = 0;
static int wcap = 0;

static int ensure_cap(void) {
    if (wcount >= wcap) {
        int newcap = wcap == 0 ? 8 : wcap * 2;
        WordEntry *tmp = realloc(words, sizeof(WordEntry) * newcap);
        if (!tmp) return -1;
        words = tmp;
        wcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_word(const char *w) {
    for (int i = 0; i < wcount; i++) {
        if (strcmp(words[i].word, w) == 0)
            return i;
    }
    return -1;
}

static int cmp_desc(const void *a, const void *b) {
    const WordEntry *wa = (const WordEntry *)a;
    const WordEntry *wb = (const WordEntry *)b;
    if (wb->count != wa->count) return wb->count - wa->count;
    return strcmp(wa->word, wb->word);
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';
        if (strlen(line) == 0) continue;
        int idx = find_word(line);
        if (idx >= 0) {
            words[idx].count++;
        } else {
            if (ensure_cap() != 0) continue;
            words[wcount].word = dup_str(line);
            if (!words[wcount].word) continue;
            words[wcount].count = 1;
            wcount++;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "WORD") == 0) {
            const char *text = line + offset;
            while (*text == ' ') text++;
            if (strlen(text) == 0) continue;
            int idx = find_word(text);
            if (idx >= 0) {
                words[idx].count++;
            } else {
                if (ensure_cap() != 0) continue;
                words[wcount].word = dup_str(text);
                if (!words[wcount].word) continue;
                words[wcount].count = 1;
                wcount++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            const char *text = line + offset;
            while (*text == ' ') text++;
            int idx = find_word(text);
            if (idx < 0) continue;
            free(words[idx].word);
            for (int i = idx; i < wcount - 1; i++)
                words[i] = words[i + 1];
            wcount--;
        } else if (strcmp(cmd, "COUNT") == 0) {
            const char *text = line + offset;
            while (*text == ' ') text++;
            int idx = find_word(text);
            if (idx >= 0) {
                printf("%d\n", words[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(words, wcount, sizeof(WordEntry), cmp_desc);
            for (int i = 0; i < wcount; i++) {
                printf("%s %d\n", words[i].word, words[i].count);
            }
        }
    }
    for (int i = 0; i < wcount; i++)
        free(words[i].word);
    free(words);
    return 0;
}