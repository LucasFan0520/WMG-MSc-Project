// F002.c
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

typedef struct { char *tag; char *msg; } Entry;
static Entry *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Entry *t = realloc(arr, nc * sizeof(Entry));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_first(const char *tag) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].tag, tag) == 0) return (int)i;
    return -1;
}

static void cmd_add(char *tag, char *msg) {
    if (grow() < 0) return;
    arr[cnt].tag = safe_strdup(tag);
    arr[cnt].msg = safe_strdup(msg);
    if (!arr[cnt].tag || !arr[cnt].msg) {
        free(arr[cnt].tag); free(arr[cnt].msg);
        arr[cnt].tag = NULL; arr[cnt].msg = NULL;
        return;
    }
    cnt++;
}

static void cmd_concat(char *t1, char *t2, char *nt) {
    int i1 = find_first(t1);
    int i2 = find_first(t2);
    if (i1 < 0 || i2 < 0) return;
    size_t l1 = strlen(arr[i1].msg);
    size_t l2 = strlen(arr[i2].msg);
    if (l1 > (size_t)-1 - l2 - 2) return;
    char *combined = malloc(l1 + 1 + l2 + 1);
    if (!combined) return;
    memcpy(combined, arr[i1].msg, l1);
    combined[l1] = ' ';
    memcpy(combined + l1 + 1, arr[i2].msg, l2 + 1);
    cmd_add(nt, combined);
    free(combined);
}

static void cmd_remove(char *tag) {
    size_t w = 0;
    for (size_t r = 0; r < cnt; r++) {
        if (strcmp(arr[r].tag, tag) == 0) {
            free(arr[r].tag); free(arr[r].msg);
        } else {
            arr[w++] = arr[r];
        }
    }
    cnt = w;
}

static void cmd_find(char *tag) {
    int idx = find_first(tag);
    if (idx < 0) return;
    for (char *p = arr[idx].msg; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s ", arr[i].tag);
        for (char *p = arr[i].msg; *p; p++)
            putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].tag); free(arr[i].msg); }
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
            char tag[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", tag, &n2) < 1) continue;
            char *msg = rest + n2;
            while (*msg == ' ') msg++;
            cmd_add(tag, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[256] = {0}, t2[256] = {0}, nt[256] = {0};
            if (sscanf(rest, "%255s %255s %255s", t1, t2, nt) < 3) continue;
            cmd_concat(t1, t2, nt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256] = {0};
            if (sscanf(rest, "%255s", tag) < 1) continue;
            cmd_remove(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256] = {0};
            if (sscanf(rest, "%255s", tag) < 1) continue;
            cmd_find(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}