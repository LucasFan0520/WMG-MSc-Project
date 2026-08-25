/*
 * F030.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rule {
    char *old;
    char *new;
    struct rule *next;
} Rule;

Rule *rules = NULL;

int count_rules(void) {
    int n = 0;
    Rule *r = rules;
    while (r) { n++; r = r->next; }
    return n;
}

Rule *get_rule_at(int index) {
    Rule *r = rules;
    int i;
    for (i = 0; r && i < index; i++) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = rules;
    rules = r;
}

void apply_rules(const char *text) {
    Rule *r = rules;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            char result[4096];
            strcpy(result, text);
            // simple replacement: only first occurrence
            char tmp[4096];
            strcpy(tmp, pos + strlen(r->old));
            *pos = 0;
            strcat(result, r->new);
            strcat(result, tmp);
            printf("%s\n", result);
            return;
        }
        r = r->next;
    }
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    Rule *prev = NULL, *r = rules;
    while (r) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else rules = r->next;
            free(r->old);
            free(r->new);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void swap_rules(int i, int j) {
    int n = count_rules();
    if (i < 0 || i >= n || j < 0 || j >= n || i == j) return;
    if (i > j) { int tmp = i; i = j; j = tmp; }
    Rule *prev_i = NULL, *ri = rules;
    int k;
    for (k = 0; k < i; k++) { prev_i = ri; ri = ri->next; }
    Rule *prev_j = ri, *rj = ri;
    for (k = i; k < j; k++) { prev_j = rj; rj = rj->next; }
    // swap nodes
    if (prev_i) prev_i->next = rj;
    else rules = rj;
    prev_j->next = ri;
    Rule *tmp_next = rj->next;
    rj->next = ri->next;
    ri->next = tmp_next;
}

void print_rules(void) {
    Rule *r = rules;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        if (sscanf(line, "%31s %255s", cmd, arg1) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            add_rule(arg1, rest);
        } else if (strcmp(cmd, "APPLY") == 0) {
            apply_rules(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(line, "%*s %d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    return 0;
}