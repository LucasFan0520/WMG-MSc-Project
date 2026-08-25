/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
} Rule;

Rule *rules = NULL;
int rule_count = 0;
int rule_capacity = 0;

void ensure_rule_capacity(int n) {
    if (n >= rule_capacity) {
        rule_capacity = rule_capacity ? rule_capacity * 2 : 4;
        while (rule_capacity <= n) rule_capacity *= 2;
        rules = realloc(rules, rule_capacity * sizeof(Rule));
    }
}

void add_rule(const char *old, const char *new) {
    ensure_rule_capacity(rule_count + 1);
    rules[rule_count].old = strdup(old);
    rules[rule_count].new = strdup(new);
    rule_count++;
}

void delete_rule(const char *old) {
    for (int i = 0; i < rule_count; i++) {
        if (strcmp(rules[i].old, old) == 0) {
            free(rules[i].old);
            free(rules[i].new);
            for (int j = i; j < rule_count - 1; j++) {
                rules[j] = rules[j+1];
            }
            rule_count--;
            return;
        }
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count) return;
    Rule tmp = rules[i];
    rules[i] = rules[j];
    rules[j] = tmp;
}

void apply_rule(const char *text) {
    for (int i = 0; i < rule_count; i++) {
        char *pos = strstr(text, rules[i].old);
        if (pos) {
            char *result = malloc(strlen(text) + strlen(rules[i].new) + 1);
            char *p = result;
            while (*text) {
                if (text == pos) {
                    strcpy(p, rules[i].new);
                    p += strlen(rules[i].new);
                    text += strlen(rules[i].old);
                } else {
                    *p++ = *text++;
                }
            }
            *p = '\0';
            printf("%s\n", result);
            free(result);
            return;
        }
    }
}

void print_rules(void) {
    for (int i = 0; i < rule_count; i++) {
        printf("%s %s\n", rules[i].old, rules[i].new);
    }
}

void free_rules(void) {
    for (int i = 0; i < rule_count; i++) {
        free(rules[i].old);
        free(rules[i].new);
    }
    free(rules);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char old[256], new[256];
        int i, j;
        int n = sscanf(line, "%s %s %s %d %d", cmd, old, new, &i, &j);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1;
            while (*p == ' ') p++;
            if (*p) apply_rule(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (n < 3) continue;
            swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_rules();
    return 0;
}