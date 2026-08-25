// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *old; char *new; } Rule;
Rule *rules = NULL;
int rc = 0, rcap = 0;

void add_rule(const char *old, const char *new) {
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; rules = realloc(rules, sizeof(Rule) * rcap); }
    rules[rc].old = strdup(old);
    rules[rc].new = strdup(new);
    rc++;
}

void apply_rules(const char *text) {
    for (int i = 0; i < rc; i++) {
        if (strcmp(text, rules[i].old) == 0) {
            printf("%s\n", rules[i].new);
            return;
        }
    }
    // If no match, print text unchanged
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    for (int i = 0; i < rc; i++) {
        if (strcmp(rules[i].old, old) == 0) {
            free(rules[i].old);
            free(rules[i].new);
            for (int j = i; j < rc - 1; j++) rules[j] = rules[j + 1];
            rc--;
            return;
        }
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rc || j < 0 || j >= rc) return;
    Rule tmp = rules[i];
    rules[i] = rules[j];
    rules[j] = tmp;
}

void print_rules(void) {
    for (int i = 0; i < rc; i++)
        printf("%s %s\n", rules[i].old, rules[i].new);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        int i, j;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", arg1) == 1)
                apply_rules(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) print_rules();
    }
    for (int i = 0; i < rc; i++) { free(rules[i].old); free(rules[i].new); }
    free(rules);
    return 0;
}