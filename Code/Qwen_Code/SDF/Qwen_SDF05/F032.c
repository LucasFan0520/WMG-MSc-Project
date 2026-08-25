// F032.c
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

typedef struct { char *key; char *value; } Setting;

static Setting *committed = NULL;
static size_t ccnt = 0, ccap = 0;

static Setting *staged = NULL;
static size_t scnt = 0, scap = 0;

static int in_transaction = 0;

static int c_grow(void) {
    if (ccnt < ccap) return 0;
    size_t nc = ccap ? ccap * 2 : 8;
    if (nc < ccap) return -1;
    Setting *t = realloc(committed, nc * sizeof(Setting));
    if (!t) return -1;
    committed = t; ccap = nc;
    return 0;
}

static int s_grow(void) {
    if (scnt < scap) return 0;
    size_t nc = scap ? scap * 2 : 8;
    if (nc < scap) return -1;
    Setting *t = realloc(staged, nc * sizeof(Setting));
    if (!t) return -1;
    staged = t; scap = nc;
    return 0;
}

static int find_c(const char *key) {
    for (size_t i = 0; i < ccnt; i++)
        if (strcmp(committed[i].key, key) == 0) return (int)i;
    return -1;
}

static int find_s(const char *key) {
    for (size_t i = 0; i < scnt; i++)
        if (strcmp(staged[i].key, key) == 0) return (int)i;
    return -1;
}

static void set_val(Setting *arr, size_t cnt, size_t cap, const char *key, const char *val) {
    (void)cap;
    for (size_t i = 0; i < cnt; i++) {
        if (strcmp(arr[i].key, key) == 0) {
            char *nv = safe_strdup(val);
            if (!nv) return;
            free(arr[i].value);
            arr[i].value = nv;
            return;
        }
    }
}

static void cmd_set(char *key, char *val) {
    if (in_transaction) {
        int idx = find_s(key);
        if (idx >= 0) {
            char *nv = safe_strdup(val);
            if (!nv) return;
            free(staged[idx].value);
            staged[idx].value = nv;
            return;
        }
        if (s_grow() < 0) return;
        staged[scnt].key = safe_strdup(key);
        staged[scnt].value = safe_strdup(val);
        if (!staged[scnt].key || !staged[scnt].value) {
            free(staged[scnt].key); free(staged[scnt].value);
            staged[scnt].key = NULL; staged[scnt].value = NULL;
            return;
        }
        scnt++;
    } else {
        int idx = find_c(key);
        if (idx >= 0) {
            char *nv = safe_strdup(val);
            if (!nv) return;
            free(committed[idx].value);
            committed[idx].value = nv;
            return;
        }
        if (c_grow() < 0) return;
        committed[ccnt].key = safe_strdup(key);
        committed[ccnt].value = safe_strdup(val);
        if (!committed[ccnt].key || !committed[ccnt].value) {
            free(committed[ccnt].key); free(committed[ccnt].value);
            committed[ccnt].key = NULL; committed[ccnt].value = NULL;
            return;
        }
        ccnt++;
    }
}

static void cmd_begin(void) {
    in_transaction = 1;
    for (size_t i = 0; i < scnt; i++) { free(staged[i].key); free(staged[i].value); }
    scnt = 0;
}

static void cmd_commit(void) {
    for (size_t i = 0; i < scnt; i++) {
        int idx = find_c(staged[i].key);
        if (idx >= 0) {
            free(committed[idx].value);
            committed[idx].value = staged[i].value;
            free(staged[i].key);
        } else {
            if (c_grow() >= 0) {
                committed[ccnt] = staged[i];
                ccnt++;
            } else {
                free(staged[i].key); free(staged[i].value);
            }
        }
    }
    scnt = 0;
    in_transaction = 0;
}

static void cmd_abort(void) {
    for (size_t i = 0; i < scnt; i++) { free(staged[i].key); free(staged[i].value); }
    scnt = 0;
    in_transaction = 0;
}

static void cmd_get(char *key) {
    if (in_transaction) {
        int idx = find_s(key);
        if (idx >= 0) { printf("%s\n", staged[idx].value); return; }
    }
    int idx = find_c(key);
    if (idx >= 0) printf("%s\n", committed[idx].value);
}

static void cmd_dump(void) {
    for (size_t i = 0; i < ccnt; i++)
        printf("%s=%s\n", committed[i].key, committed[i].value);
}

static void cleanup(void) {
    for (size_t i = 0; i < ccnt; i++) { free(committed[i].key); free(committed[i].value); }
    free(committed);
    for (size_t i = 0; i < scnt; i++) { free(staged[i].key); free(staged[i].value); }
    free(staged);
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
        if (strcmp(cmd, "SET") == 0 || strcmp(cmd, "TSET") == 0) {
            char key[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", key, &n2) < 1) continue;
            char *val = rest + n2;
            while (*val == ' ') val++;
            if (strcmp(cmd, "TSET") == 0 && !in_transaction) cmd_begin();
            cmd_set(key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            cmd_begin();
        } else if (strcmp(cmd, "COMMIT") == 0) {
            cmd_commit();
        } else if (strcmp(cmd, "ABORT") == 0) {
            cmd_abort();
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256] = {0};
            if (sscanf(rest, "%255s", key) < 1) continue;
            cmd_get(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            cmd_dump();
        }
    }
    cleanup();
    return 0;
}