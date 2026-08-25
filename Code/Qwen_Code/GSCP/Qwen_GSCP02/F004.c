/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Var;

static Var *vars = NULL;
static int vcount = 0;
static int vcap = 0;

static int ensure_cap(void) {
    if (vcount >= vcap) {
        int newcap = vcap == 0 ? 8 : vcap * 2;
        Var *tmp = realloc(vars, sizeof(Var) * newcap);
        if (!tmp) return -1;
        vars = tmp;
        vcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_var(const char *key) {
    for (int i = 0; i < vcount; i++) {
        if (strcmp(vars[i].key, key) == 0)
            return i;
    }
    return -1;
}

static void render_text(const char *text) {
    size_t tlen = strlen(text);
    size_t i = 0;
    while (i < tlen) {
        if (text[i] == '{') {
            size_t j = i + 1;
            while (j < tlen && text[j] != '}') j++;
            if (j < tlen) {
                size_t klen = j - i - 1;
                char *key = malloc(klen + 1);
                if (!key) { putchar(text[i]); i++; continue; }
                memcpy(key, text + i + 1, klen);
                key[klen] = '\0';
                int idx = find_var(key);
                if (idx >= 0) {
                    fputs(vars[idx].value, stdout);
                } else {
                    fwrite(text + i, 1, j - i + 1, stdout);
                }
                free(key);
                i = j + 1;
            } else {
                putchar(text[i]);
                i++;
            }
        } else {
            putchar(text[i]);
            i++;
        }
    }
    putchar('\n');
}

int main(void) {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            char key[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", key, &off2) != 1) continue;
            const char *val = line + offset + off2;
            while (*val == ' ') val++;
            int idx = find_var(key);
            if (idx >= 0) {
                char *nv = dup_str(val);
                if (!nv) continue;
                free(vars[idx].value);
                vars[idx].value = nv;
            } else {
                if (ensure_cap() != 0) continue;
                vars[vcount].key = dup_str(key);
                vars[vcount].value = dup_str(val);
                if (!vars[vcount].key || !vars[vcount].value) {
                    free(vars[vcount].key);
                    free(vars[vcount].value);
                    continue;
                }
                vcount++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, " %255s", key) != 1) continue;
            int idx = find_var(key);
            if (idx < 0) continue;
            free(vars[idx].key);
            free(vars[idx].value);
            for (int i = idx; i < vcount - 1; i++)
                vars[i] = vars[i + 1];
            vcount--;
        } else if (strcmp(cmd, "RENDER") == 0) {
            const char *text = line + offset;
            while (*text == ' ') text++;
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < vcount; i++) {
                printf("%s=%s\n", vars[i].key, vars[i].value);
            }
        }
    }
    for (int i = 0; i < vcount; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}