// F010.c
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
    char *key;
    char **vals;
    size_t vcnt, vcap;
} KeyEntry;

static KeyEntry *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    KeyEntry *t = realloc(arr, nc * sizeof(KeyEntry));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int val_grow(KeyEntry *k) {
    if (k->vcnt < k->vcap) return 0;
    size_t nc = k->vcap ? k->vcap * 2 : 4;
    if (nc < k->vcap) return -1;
    char **t = realloc(k->vals, nc * sizeof(char *));
    if (!t) return -1;
    k->vals = t; k->vcap = nc;
    return 0;
}

static int find_key(const char *key) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].key, key) == 0) return (int)i;
    return -1;
}

static void free_key(KeyEntry *k) {
    for (size_t i = 0; i < k->vcnt; i++) free(k->vals[i]);
    free(k->vals);
    free(k->key);
    k->key = NULL; k->vals = NULL; k->vcnt = 0; k->vcap = 0;
}

static void cmd_add(char *key, char *value) {
    int idx = find_key(key);
    if (idx < 0) {
        if (grow() < 0) return;
        arr[cnt].key = safe_strdup(key);
        if (!arr[cnt].key) return;
        arr[cnt].vals = NULL;
        arr[cnt].vcnt = 0;
        arr[cnt].vcap = 0;
        idx = (int)cnt;
        cnt++;
    }
    KeyEntry *k = &arr[idx];
    if (val_grow(k) < 0) return;
    k->vals[k->vcnt] = safe_strdup(value);
    if (k->vals[k->vcnt]) k->vcnt++;
}

static void cmd_remove(char *key, char *value) {
    int idx = find_key(key);
    if (idx < 0) return;
    KeyEntry *k = &arr[idx];
    for (size_t i = 0; i < k->vcnt; i++) {
        if (strcmp(k->vals[i], value) == 0) {
            free(k->vals[i]);
            for (size_t j = i; j + 1 < k->vcnt; j++) k->vals[j] = k->vals[j + 1];
            k->vcnt--;
            return;
        }
    }
}

static void cmd_erase(char *key) {
    int idx = find_key(key);
    if (idx < 0) return;
    free_key(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_values(char *key) {
    int idx = find_key(key);
    if (idx < 0) return;
    for (size_t i = 0; i < arr[idx].vcnt; i++)
        printf("%s\n", arr[idx].vals[i]);
}

static void cmd_keys(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s\n", arr[i].key);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_key(&arr[i]);
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
            char key[256] = {0}, val[256] = {0};
            if (sscanf(rest, "%255s %255s", key, val) < 2) continue;
            cmd_add(key, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256] = {0}, val[256] = {0};
            if (sscanf(rest, "%255s %255s", key, val) < 2) continue;
            cmd_remove(key, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256] = {0};
            if (sscanf(rest, "%255s", key) < 1) continue;
            cmd_erase(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256] = {0};
            if (sscanf(rest, "%255s", key) < 1) continue;
            cmd_values(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            cmd_keys();
        }
    }
    cleanup();
    return 0;
}