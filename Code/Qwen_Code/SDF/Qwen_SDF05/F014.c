// F014.c
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

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t lcnt, lcap;
} Item;

static Item *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Item *t = realloc(arr, nc * sizeof(Item));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int lbl_grow(Item *it) {
    if (it->lcnt < it->lcap) return 0;
    size_t nc = it->lcap ? it->lcap * 2 : 4;
    if (nc < it->lcap) return -1;
    char **t = realloc(it->labels, nc * sizeof(char *));
    if (!t) return -1;
    it->labels = t; it->lcap = nc;
    return 0;
}

static int find_item(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_item(Item *it) {
    free(it->name); free(it->note);
    for (size_t i = 0; i < it->lcnt; i++) free(it->labels[i]);
    free(it->labels);
    it->name = NULL; it->note = NULL; it->labels = NULL;
    it->lcnt = 0; it->lcap = 0;
}

static void cmd_item(char *name, char *note) {
    if (find_item(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].note = safe_strdup(note);
    arr[cnt].labels = NULL;
    arr[cnt].lcnt = 0;
    arr[cnt].lcap = 0;
    if (!arr[cnt].name || !arr[cnt].note) {
        free_item(&arr[cnt]);
        return;
    }
    cnt++;
}

static void cmd_label(char *name, char *label) {
    int idx = find_item(name);
    if (idx < 0) return;
    Item *it = &arr[idx];
    for (size_t i = 0; i < it->lcnt; i++)
        if (strcmp(it->labels[i], label) == 0) return;
    if (lbl_grow(it) < 0) return;
    it->labels[it->lcnt] = safe_strdup(label);
    if (it->labels[it->lcnt]) it->lcnt++;
}

static void cmd_unlabel(char *name, char *label) {
    int idx = find_item(name);
    if (idx < 0) return;
    Item *it = &arr[idx];
    for (size_t i = 0; i < it->lcnt; i++) {
        if (strcmp(it->labels[i], label) == 0) {
            free(it->labels[i]);
            for (size_t j = i; j + 1 < it->lcnt; j++) it->labels[j] = it->labels[j + 1];
            it->lcnt--;
            return;
        }
    }
}

static void cmd_delete(char *name) {
    int idx = find_item(name);
    if (idx < 0) return;
    free_item(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_find(char *name) {
    int idx = find_item(name);
    if (idx < 0) return;
    printf("%s %s", arr[idx].name, arr[idx].note);
    for (size_t i = 0; i < arr[idx].lcnt; i++)
        printf(" %s", arr[idx].labels[i]);
    putchar('\n');
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s %s", arr[i].name, arr[i].note);
        for (size_t j = 0; j < arr[i].lcnt; j++)
            printf(" %s", arr[i].labels[j]);
        putchar('\n');
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_item(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *note = rest + n2;
            while (*note == ' ') note++;
            cmd_item(name, note);
        } else if (strcmp(cmd, "LABEL") == 0 || strcmp(cmd, "UNLABEL") == 0) {
            char name[256] = {0}, label[256] = {0};
            if (sscanf(rest, "%255s %255s", name, label) < 2) continue;
            if (strcmp(cmd, "LABEL") == 0) cmd_label(name, label);
            else cmd_unlabel(name, label);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(name);
            else cmd_find(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}