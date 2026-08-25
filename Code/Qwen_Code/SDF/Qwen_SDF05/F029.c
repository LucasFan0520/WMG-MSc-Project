// F029.c
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

typedef struct { char *key; char *value; } Record;
static Record *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Record *t = realloc(arr, nc * sizeof(Record));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static void cmd_add(char *key, char *value) {
    if (grow() < 0) return;
    arr[cnt].key = safe_strdup(key);
    arr[cnt].value = safe_strdup(value);
    if (!arr[cnt].key || !arr[cnt].value) {
        free(arr[cnt].key); free(arr[cnt].value);
        arr[cnt].key = NULL; arr[cnt].value = NULL;
        return;
    }
    cnt++;
}

static void cmd_dedup(void) {
    size_t w = 0;
    for (size_t r = 0; r < cnt; r++) {
        int dup = 0;
        for (size_t i = 0; i < w; i++) {
            if (strcmp(arr[i].key, arr[r].key) == 0) { dup = 1; break; }
        }
        if (dup) {
            free(arr[r].key); free(arr[r].value);
        } else {
            arr[w++] = arr[r];
        }
    }
    cnt = w;
}

static void cmd_delete(char *key) {
    size_t w = 0;
    for (size_t r = 0; r < cnt; r++) {
        if (strcmp(arr[r].key, key) == 0) {
            free(arr[r].key); free(arr[r].value);
        } else {
            arr[w++] = arr[r];
        }
    }
    cnt = w;
}

static void cmd_find(char *key) {
    for (size_t i = 0; i < cnt; i++) {
        if (strcmp(arr[i].key, key) == 0)
            printf("%s\n", arr[i].value);
    }
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s\n", arr[i].key, arr[i].value);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].key); free(arr[i].value); }
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
            char key[256] = {0}, value[256] = {0};
            if (sscanf(rest, "%255s %255s", key, value) < 2) continue;
            cmd_add(key, value);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            cmd_dedup();
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "FIND") == 0) {
            char key[256] = {0};
            if (sscanf(rest, "%255s", key) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(key);
            else cmd_find(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}