// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old_str;
    char *new_str;
    struct Rule *next;
} Rule;

Rule *rules = NULL;
int rule_count = 0;

void add_rule(const char *old_str, const char *new_str) {
    Rule *r = malloc(sizeof(Rule));
    r->old_str = strdup(old_str);
    r->new_str = strdup(new_str);
    r->next = NULL;
    if (!rules) {
        rules = r;
    } else {
        Rule *curr = rules;
        while (curr->next) curr = curr->next;
        curr->next = r;
    }
    rule_count++;
}

void apply_rule(const char *text) {
    for (Rule *r = rules; r; r = r->next) {
        char *pos = strstr(text, r->old_str);
        if (pos) {
            size_t old_len = strlen(r->old_str);
            size_t new_len = strlen(r->new_str);
            size_t text_len = strlen(text);
            size_t out_len = text_len - old_len + new_len;
            char *out = malloc(out_len + 1);
            size_t prefix_len = pos - text;
            strncpy(out, text, prefix_len);
            strcpy(out + prefix_len, r->new_str);
            strcpy(out + prefix_len + new_len, pos + old_len);
            printf("%s\n", out);
            free(out);
            return;
        }
    }
    printf("%s\n", text);
}

void delete_rule(const char *old_str) {
    Rule **rp = &rules;
    while (*rp) {
        if (strcmp((*rp)->old_str, old_str) == 0) {
            Rule *tmp = *rp;
            *rp = tmp->next;
            free(tmp->old_str);
            free(tmp->new_str);
            free(tmp);
            rule_count--;
            return;
        }
        rp = &(*rp)->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    Rule *pi = NULL, *ci = rules;
    for (int k = 0; k < i; k++) { pi = ci; ci = ci->next; }
    Rule *pj = NULL, *cj = rules;
    for (int k = 0; k < j; k++) { pj = cj; cj = cj->next; }
    
    Rule *ci_next = ci->next;
    Rule *cj_next = cj->next;
    
    if (pi) pi->next = cj; else rules = cj;
    cj->next = (cj_next == ci) ? ci : ci_next;
    
    if (pj) pj->next = ci; else rules = ci;
    ci->next = (ci_next == cj) ? cj : cj_next;
}

void print_rules() {
    for (Rule *r = rules; r; r = r->next) {
        printf("%s %s\n", r->old_str, r->new_str);
    }
}

void cleanup() {
    while (rules) {
        Rule *tmp = rules->next;
        free(rules->old_str);
        free(rules->new_str);
        free(rules);
        rules = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        int i, j;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) add_rule(a1, a2);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *t = strchr(line, ' ');
            if (t) apply_rule(t + 1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_rule(a1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    cleanup();
    return 0;
}