// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *rules = NULL;
int rule_count = 0;

Rule *get_rule_at(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *r = rules;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = rules;
    rules = r;
    rule_count++;
}

void apply_rules(const char *text) {
    Rule *r = rules;
    while (r) {
        if (strcmp(r->old, text) == 0) {
            printf("%s\n", r->new);
            return;
        }
        r = r->next;
    }
    // no match, print original? spec says apply first matching rule, if none? It doesn't specify, likely print text unchanged.
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    Rule *cur = rules, *prev = NULL;
    while (cur) {
        if (strcmp(cur->old, old) == 0) {
            if (prev) prev->next = cur->next;
            else rules = cur->next;
            free(cur->old);
            free(cur->new);
            free(cur);
            rule_count--;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void swap_rules(int i, int j) {
    if (i == j) return;
    Rule *ri = get_rule_at(i);
    Rule *rj = get_rule_at(j);
    if (!ri || !rj) return;
    // swap old and new
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void print_rules(void) {
    Rule *r = rules;
    int idx = 0;
    while (r) {
        printf("%d %s %s\n", idx, r->old, r->new);
        r = r->next;
        idx++;
    }
}

void cleanup_rules(void) {
    Rule *r = rules;
    while (r) {
        Rule *tmp = r;
        r = r->next;
        free(tmp->old);
        free(tmp->new);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = line + strlen(cmd) + 1;
            apply_rules(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0 && n >= 3) {
            int i = atoi(arg1);
            int j = atoi(arg2);
            swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    cleanup_rules();
    return 0;
}