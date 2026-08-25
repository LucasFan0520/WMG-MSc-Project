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

Rule *find_rule_by_old(const char *old) {
    Rule *r = rules;
    while (r) {
        if (strcmp(r->old, old) == 0) return r;
        r = r->next;
    }
    return NULL;
}

Rule *get_rule_at(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *r = rules;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    if (find_rule_by_old(old)) return;
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = rules;
    rules = r;
    rule_count++;
}

void apply_rule(const char *text) {
    Rule *r = rules;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // replace first occurrence
            char *result = malloc(strlen(text) - strlen(r->old) + strlen(r->new) + 1);
            strncpy(result, text, pos - text);
            result[pos - text] = '\0';
            strcat(result, r->new);
            strcat(result, pos + strlen(r->old));
            for (char *s = result; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
            free(result);
            return;
        }
        r = r->next;
    }
    // no match: print original with spaces as underscores
    for (char *s = text; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void delete_rule(const char *old) {
    Rule *prev = NULL;
    Rule *r = rules;
    while (r) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else rules = r->next;
            free(r->old);
            free(r->new);
            free(r);
            rule_count--;
            return;
        }
        prev = r;
        r = r->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    // find nodes and their predecessors
    Rule *prev_i = NULL, *prev_j = NULL;
    Rule *node_i = rules, *node_j = rules;
    for (int k = 0; k < i; k++) { prev_i = node_i; node_i = node_i->next; }
    for (int k = 0; k < j; k++) { prev_j = node_j; node_j = node_j->next; }
    if (!node_i || !node_j) return;
    // swap
    if (prev_i) prev_i->next = node_j;
    else rules = node_j;
    if (prev_j) prev_j->next = node_i;
    else rules = node_i;
    Rule *tmp = node_i->next;
    node_i->next = node_j->next;
    node_j->next = tmp;
}

void print_rules() {
    Rule *r = rules;
    while (r) {
        printf("%s->%s\n", r->old, r->new);
        r = r->next;
    }
}

void free_all() {
    Rule *r = rules;
    while (r) {
        Rule *next = r->next;
        free(r->old);
        free(r->new);
        free(r);
        r = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char old[256], new[4096];
            if (sscanf(rest, "%255s %[^\n]", old, new) == 2) {
                add_rule(old, new);
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            apply_rule(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_rule(rest);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(rest, "%d %d", &i, &j) == 2) {
                swap_rules(i, j);
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_all();
    return 0;
}