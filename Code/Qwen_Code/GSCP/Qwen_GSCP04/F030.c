// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule { char *old; char *new; } Rule;
Rule *rules = NULL; int count = 0, cap = 0;

void ensure_cap() { if (count == cap) { cap = cap ? cap * 2 : 4; rules = realloc(rules, cap * sizeof(Rule)); } }
void cmd_add(char *o, char *n) { ensure_cap(); rules[count].old = strdup(o); rules[count].new = strdup(n); count++; }
void cmd_apply(char *t) {
    for (int i = 0; i < count; i++) {
        char *pos = strstr(t, rules[i].old);
        if (pos) {
            size_t olen = strlen(rules[i].old), nlen = strlen(rules[i].new);
            size_t tlen = strlen(t);
            char *res = malloc(tlen - olen + nlen + 1);
            memcpy(res, t, pos - t);
            memcpy(res + (pos - t), rules[i].new, nlen);
            strcpy(res + (pos - t) + nlen, pos + olen);
            printf("%s\n", res); free(res); return;
        }
    }
    printf("%s\n", t);
}
void cmd_delete(char *o) {
    for (int i = 0; i < count; i++) {
        if (!strcmp(rules[i].old, o)) {
            free(rules[i].old); free(rules[i].new);
            for (int j = i; j < count - 1; j++) rules[j] = rules[j+1];
            count--; return;
        }
    }
}
void cmd_swap(int i, int j) {
    if (i < 0 || i >= count || j < 0 || j >= count) return;
    Rule tmp = rules[i]; rules[i] = rules[j]; rules[j] = tmp;
}
void cmd_rules() { for (int i = 0; i < count; i++) printf("%s %s\n", rules[i].old, rules[i].new); }
void free_all() { for (int i = 0; i < count; i++) { free(rules[i].old); free(rules[i].new); } free(rules); }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256]; int i, j;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_add(a1, a2);
        else if (!strcmp(cmd, "APPLY")) { char *t = strchr(line, ' '); if (t) cmd_apply(t + 1); }
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "SWAP") && sscanf(line, "%*s %d %d", &i, &j) == 2) cmd_swap(i, j);
        else if (!strcmp(cmd, "RULES")) cmd_rules();
    }
    free_all(); return 0;
}