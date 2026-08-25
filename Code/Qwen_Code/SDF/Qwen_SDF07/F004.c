// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *key; char *val; } Var;
Var *v = NULL;
size_t vc = 0, vcap = 0;

void set_v(const char *key, const char *val) {
    for (size_t i = 0; i < vc; i++) {
        if (strcmp(v[i].key, key) == 0) {
            char *tmp = strdup(val);
            if (!tmp) { perror("strdup"); exit(1); }
            free(v[i].val);
            v[i].val = tmp;
            return;
        }
    }
    if (vc == vcap) {
        size_t nc = vcap ? vcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Var)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Var *tmp = realloc(v, nc * sizeof(Var));
        if (!tmp) { perror("realloc"); exit(1); }
        v = tmp;
        vcap = nc;
    }
    v[vc].key = strdup(key);
    v[vc].val = strdup(val);
    if (!v[vc].key || !v[vc].val) { perror("strdup"); exit(1); }
    vc++;
}

void unset_v(const char *key) {
    for (size_t i = 0; i < vc; i++) {
        if (strcmp(v[i].key, key) == 0) {
            free(v[i].key); free(v[i].val);
            v[i] = v[--vc];
            return;
        }
    }
}

void render(const char *text) {
    size_t cap = strlen(text) * 2 + 1;
    char *out = malloc(cap);
    if (!out) { perror("malloc"); exit(1); }
    size_t olen = 0;
    for (size_t i = 0; text[i]; ) {
        if (text[i] == '{') {
            size_t j = i + 1;
            while (text[j] && text[j] != '}') j++;
            if (text[j] == '}') {
                char key[256];
                size_t klen = j - i - 1;
                if (klen >= sizeof(key)) klen = sizeof(key) - 1;
                strncpy(key, text + i + 1, klen);
                key[klen] = '\0';
                int found = 0;
                for (size_t k = 0; k < vc; k++) {
                    if (strcmp(v[k].key, key) == 0) {
                        size_t vlen = strlen(v[k].val);
                        while (olen + vlen + 1 >= cap) {
                            if (cap > SIZE_MAX / 2) { free(out); fprintf(stderr, "Overflow\n"); exit(1); }
                            cap *= 2;
                            char *tmp = realloc(out, cap);
                            if (!tmp) { free(out); perror("realloc"); exit(1); }
                            out = tmp;
                        }
                        memcpy(out + olen, v[k].val, vlen);
                        olen += vlen;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    while (olen + (j - i) + 1 >= cap) {
                        if (cap > SIZE_MAX / 2) { free(out); fprintf(stderr, "Overflow\n"); exit(1); }
                        cap *= 2;
                        char *tmp = realloc(out, cap);
                        if (!tmp) { free(out); perror("realloc"); exit(1); }
                        out = tmp;
                    }
                    memcpy(out + olen, text + i, j - i + 1);
                    olen += j - i + 1;
                }
                i = j + 1;
            } else {
                while (olen + 2 >= cap) {
                    if (cap > SIZE_MAX / 2) { free(out); fprintf(stderr, "Overflow\n"); exit(1); }
                    cap *= 2;
                    char *tmp = realloc(out, cap);
                    if (!tmp) { free(out); perror("realloc"); exit(1); }
                    out = tmp;
                }
                out[olen++] = text[i++];
            }
        } else {
            while (olen + 2 >= cap) {
                if (cap > SIZE_MAX / 2) { free(out); fprintf(stderr, "Overflow\n"); exit(1); }
                cap *= 2;
                char *tmp = realloc(out, cap);
                if (!tmp) { free(out); perror("realloc"); exit(1); }
                out = tmp;
            }
            out[olen++] = text[i++];
        }
    }
    out[olen] = '\0';
    printf("%s\n", out);
    free(out);
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) != 1) { free(line); continue; }
            char *val = strchr(line, ' ');
            if (val) { val = strchr(val + 1, ' '); if (val) val++; }
            if (!val) val = "";
            set_v(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) unset_v(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = strchr(line, ' ');
            if (text) text++;
            if (text) render(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < vc; i++) printf("%s=%s\n", v[i].key, v[i].val);
        }
        free(line);
    }
    for (size_t i = 0; i < vc; i++) { free(v[i].key); free(v[i].val); }
    free(v);
    return 0;
}