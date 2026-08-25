// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *category; char *text; } Note;
static Note *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Note *t = realloc(arr, nc * sizeof(Note));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static void cmd_add(char *category, char *text) {
    if (grow() < 0) return;
    arr[cnt].category = safe_strdup(category);
    arr[cnt].text = safe_strdup(text);
    if (!arr[cnt].category || !arr[cnt].text) {
        free(arr[cnt].category); free(arr[cnt].text);
        arr[cnt].category = NULL; arr[cnt].text = NULL;
        return;
    }
    cnt++;
}

static void cmd_delete(char *category) {
    size_t w = 0;
    for (size_t r = 0; r < cnt; r++) {
        if (strcmp(arr[r].category, category) == 0) {
            free(arr[r].category); free(arr[r].text);
        } else {
            arr[w++] = arr[r];
        }
    }
    cnt = w;
}

static void cmd_keep(char *category) {
    size_t w = 0;
    for (size_t r = 0; r < cnt; r++) {
        if (strcmp(arr[r].category, category) != 0) {
            free(arr[r].category); free(arr[r].text);
        } else {
            arr[w++] = arr[r];
        }
    }
    cnt = w;
}

static void cmd_find(char *word) {
    for (size_t i = 0; i < cnt; i++) {
        if (strstr(arr[i].text, word) != NULL) {
            printf("%s %s\n", arr[i].category, arr[i].text);
        }
    }
}

static void cmd_print(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s\n", arr[i].category, arr[i].text);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].category); free(arr[i].text); }
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
        if (strcmp(cmd, "ADD") == 0) {
            char category[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", category, &n2) < 1) continue;
            char *text = rest + n2;
            while (*text == ' ') text++;
            cmd_add(category, text);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "KEEP") == 0 || strcmp(cmd, "FIND") == 0) {
            char arg[256] = {0};
            if (sscanf(rest, "%255s", arg) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(arg);
            else if (strcmp(cmd, "KEEP") == 0) cmd_keep(arg);
            else cmd_find(arg);
        } else if (strcmp(cmd, "PRINT") == 0) {
            cmd_print();
        }
    }
    cleanup();
    return 0;
}