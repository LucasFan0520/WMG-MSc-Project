// F030.c
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

typedef struct { char *old; char *new; } Rule;
Rule *rules = NULL;
size_t rc = 0, rcap = 0;

void add_rule(const char *old, const char *new) {
    if (rc == rcap) {
        size_t nc = rcap ? rcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Rule)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Rule *tmp = realloc(rules, nc * sizeof(Rule));
        if (!tmp) { perror("realloc"); exit(1); }
        rules = tmp;
        rcap = nc;
    }
    rules[rc].old = strdup(old);
    rules[rc].new = strdup(new);
    if (!rules[rc].old || !rules[rc].new) { perror("strdup"); exit(1); }
    rc++;
}

void del_rule(const char *old) {
    for (size_t i = 0; i < rc; i++) {
        if (strcmp(rules[i].old, old) == 0) {
            free(rules[i].old); free(rules[i].new);
            rules[i] = rules[--rc];
            return;
        }
    }
}

void swap_rule(size_t i, size_t j) {
    if (i >= rc || j >= rc || i == j) return;
    Rule tmp = rules[i];
    rules[i] = rules[j];
    rules[j] = tmp;
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char old[256], new_str[256];
            if (sscanf(line, "%*s %255s %255s", old, new_str) == 2) add_rule(old, new_str);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = strchr(line, ' ');
            if (text) text++;
            if (text) {
                int applied = 0;
                for (size_t i = 0; i < rc; i++) {
                    char *pos = strstr(text, rules[i].old);
                    if (pos) {
                        size_t pre = pos - text;
                        size_t old_len = strlen(rules[i].old);
                        size_t new_len = strlen(rules[i].new);
                        size_t total = pre + new_len + strlen(pos + old_len) + 1;
                        char *res = malloc(total);
                        if (!res) { perror("malloc"); exit(1); }
                        memcpy(res, text, pre);
                        memcpy(res + pre, rules[i].new, new_len);
                        strcpy(res + pre + new_len, pos + old_len);
                        printf("%s\n", res);
                        free(res);
                        applied = 1;
                        break;
                    }
                }
                if (!applied) printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old[256];
            if (sscanf(line, "%*s %255s", old) == 1) del_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            size_t i, j;
            if (sscanf(line, "%*s %zu %zu", &i, &j) == 2) swap_rule(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < rc; i++) printf("%s %s\n", rules[i].old, rules[i].new);
        }
        free(line);
    }
    for (size_t i = 0; i < rc; i++) { free(rules[i].old); free(rules[i].new); }
    free(rules);
    return 0;
}