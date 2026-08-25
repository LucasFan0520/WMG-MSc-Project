// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *word; long count; } WordEntry;
static WordEntry *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 16;
    if (nc < cap) return -1;
    WordEntry *t = realloc(arr, nc * sizeof(WordEntry));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_word(const char *word) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].word, word) == 0) return (int)i;
    return -1;
}

static void add_word(const char *word) {
    int idx = find_word(word);
    if (idx >= 0) { arr[idx].count++; return; }
    if (grow() < 0) return;
    arr[cnt].word = safe_strdup(word);
    if (!arr[cnt].word) return;
    arr[cnt].count = 1;
    cnt++;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (*line) add_word(line);
    }
    fclose(f);
}

static void cmd_word(char *text) {
    add_word(text);
}

static void cmd_remove(char *text) {
    int idx = find_word(text);
    if (idx < 0) return;
    free(arr[idx].word);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_count(char *text) {
    int idx = find_word(text);
    if (idx < 0) { printf("0\n"); return; }
    printf("%ld\n", arr[idx].count);
}

static int cmp_top(const void *a, const void *b) {
    const WordEntry *x = (const WordEntry *)a;
    const WordEntry *y = (const WordEntry *)b;
    if (x->count != y->count) return (x->count < y->count) - (x->count > y->count);
    return strcmp(x->word, y->word);
}

static void cmd_top(void) {
    if (cnt == 0) return;
    WordEntry *sorted = malloc(cnt * sizeof(WordEntry));
    if (!sorted) return;
    memcpy(sorted, arr, cnt * sizeof(WordEntry));
    qsort(sorted, cnt, sizeof(WordEntry), cmp_top);
    for (size_t i = 0; i < cnt; i++)
        printf("%s %ld\n", sorted[i].word, sorted[i].count);
    free(sorted);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free(arr[i].word);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(int argc, char *argv[]) {
    if (argc >= 2) load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "WORD") == 0) {
            cmd_word(rest);
        } else if (strcmp(cmd, "REMOVE") == 0 || strcmp(cmd, "COUNT") == 0) {
            char text[256] = {0};
            if (sscanf(rest, "%255s", text) < 1) continue;
            if (strcmp(cmd, "REMOVE") == 0) cmd_remove(text);
            else cmd_count(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            cmd_top();
        }
    }
    cleanup();
    return 0;
}