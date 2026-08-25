// F017.c
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

typedef struct { char *key; char *val; } KV;
static KV *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    KV *t = realloc(arr, nc * sizeof(KV));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_key(const char *key) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].key, key) == 0) return (int)i;
    return -1;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        if (!*key) continue;
        int idx = find_key(key);
        if (idx >= 0) {
            char *nv = safe_strdup(val);
            if (!nv) continue;
            free(arr[idx].val);
            arr[idx].val = nv;
        } else {
            if (grow() < 0) break;
            arr[cnt].key = safe_strdup(key);
            arr[cnt].val = safe_strdup(val);
            if (!arr[cnt].key || !arr[cnt].val) {
                free(arr[cnt].key); free(arr[cnt].val);
                arr[cnt].key = NULL; arr[cnt].val = NULL;
                break;
            }
            cnt++;
        }
    }
    fclose(f);
}

static void cmd_set(char *key, char *val) {
    int idx = find_key(key);
    if (idx >= 0) {
        char *nv = safe_strdup(val);
        if (!nv) return;
        free(arr[idx].val);
        arr[idx].val = nv;
        return;
    }
    if (grow() < 0) return;
    arr[cnt].key = safe_strdup(key);
    arr[cnt].val = safe_strdup(val);
    if (!arr[cnt].key || !arr[cnt].val) {
        free(arr[cnt].key); free(arr[cnt].val);
        arr[cnt].key = NULL; arr[cnt].val = NULL;
        return;
    }
    cnt++;
}

static void cmd_unset(char *key) {
    int idx = find_key(key);
    if (idx < 0) return;
    free(arr[idx].key); free(arr[idx].val);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_get(char *key) {
    int idx = find_key(key);
    if (idx < 0) return;
    printf("%s\n", arr[idx].val);
}

static void cmd_write(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s=%s\n", arr[i].key, arr[i].val);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].key); free(arr[i].val); }
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", key, &n2) < 1) continue;
            char *val = rest + n2;
            while (*val == ' ') val++;
            cmd_set(key, val);
        } else if (strcmp(cmd, "UNSET") == 0 || strcmp(cmd, "GET") == 0) {
            char key[256] = {0};
            if (sscanf(rest, "%255s", key) < 1) continue;
            if (strcmp(cmd, "UNSET") == 0) cmd_unset(key);
            else cmd_get(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            cmd_write();
        }
    }
    cleanup();
    return 0;
}