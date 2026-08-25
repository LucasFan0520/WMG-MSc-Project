// F004.c
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

typedef struct { char *key; char *val; } Var;
static Var *vars = NULL;
static size_t vcnt = 0, vcap = 0;

static int vgrow(void) {
    if (vcnt < vcap) return 0;
    size_t nc = vcap ? vcap * 2 : 8;
    if (nc < vcap) return -1;
    Var *t = realloc(vars, nc * sizeof(Var));
    if (!t) return -1;
    vars = t; vcap = nc;
    return 0;
}

static int find_var(const char *key) {
    for (size_t i = 0; i < vcnt; i++)
        if (strcmp(vars[i].key, key) == 0) return (int)i;
    return -1;
}

static void cmd_set(char *key, char *val) {
    int idx = find_var(key);
    if (idx >= 0) {
        char *nv = safe_strdup(val);
        if (!nv) return;
        free(vars[idx].val);
        vars[idx].val = nv;
        return;
    }
    if (vgrow() < 0) return;
    vars[vcnt].key = safe_strdup(key);
    vars[vcnt].val = safe_strdup(val);
    if (!vars[vcnt].key || !vars[vcnt].val) {
        free(vars[vcnt].key); free(vars[vcnt].val);
        vars[vcnt].key = NULL; vars[vcnt].val = NULL;
        return;
    }
    vcnt++;
}

static void cmd_unset(char *key) {
    int idx = find_var(key);
    if (idx < 0) return;
    free(vars[idx].key); free(vars[idx].val);
    for (size_t i = (size_t)idx; i + 1 < vcnt; i++) vars[i] = vars[i + 1];
    vcnt--;
}

static void cmd_render(char *text) {
    size_t out_cap = strlen(text) * 2 + 1;
    char *out = malloc(out_cap);
    if (!out) return;
    size_t olen = 0;
    char *p = text;
    while (*p) {
        if (*p == '{') {
            char *end = strchr(p + 1, '}');
            if (end) {
                size_t klen = (size_t)(end - p - 1);
                char *key = malloc(klen + 1);
                if (key) {
                    memcpy(key, p + 1, klen);
                    key[klen] = '\0';
                    int idx = find_var(key);
                    free(key);
                    if (idx >= 0) {
                        size_t vlen = strlen(vars[idx].val);
                        while (olen + vlen + 1 > out_cap) {
                            if (out_cap > (size_t)-1 / 2) { free(out); return; }
                            out_cap *= 2;
                            char *tmp = realloc(out, out_cap);
                            if (!tmp) { free(out); return; }
                            out = tmp;
                        }
                        memcpy(out + olen, vars[idx].val, vlen);
                        olen += vlen;
                        p = end + 1;
                        continue;
                    }
                }
            }
        }
        if (olen + 2 > out_cap) {
            if (out_cap > (size_t)-1 / 2) { free(out); return; }
            out_cap *= 2;
            char *tmp = realloc(out, out_cap);
            if (!tmp) { free(out); return; }
            out = tmp;
        }
        out[olen++] = *p++;
    }
    out[olen] = '\0';
    printf("%s\n", out);
    free(out);
}

static void cmd_vars(void) {
    for (size_t i = 0; i < vcnt; i++)
        printf("%s=%s\n", vars[i].key, vars[i].val);
}

static void cleanup(void) {
    for (size_t i = 0; i < vcnt; i++) { free(vars[i].key); free(vars[i].val); }
    free(vars); vars = NULL; vcnt = 0; vcap = 0;
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
        if (strcmp(cmd, "SET") == 0) {
            char key[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", key, &n2) < 1) continue;
            char *val = rest + n2;
            while (*val == ' ') val++;
            cmd_set(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256] = {0};
            if (sscanf(rest, "%255s", key) < 1) continue;
            cmd_unset(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            cmd_render(rest);
        } else if (strcmp(cmd, "VARS") == 0) {
            cmd_vars();
        }
    }
    cleanup();
    return 0;
}