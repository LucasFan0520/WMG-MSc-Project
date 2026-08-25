/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *head = NULL;
Rule *tail = NULL;
int rule_count = 0;

void add_rule(char *old, char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = NULL;
    if (!tail) head = tail = r;
    else { tail->next = r; tail = r; }
    rule_count++;
}

void apply_rule(char *text) {
    for (Rule *r = head; r; r = r->next) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // replace first occurrence
            size_t oldlen = strlen(r->old);
            size_t newlen = strlen(r->new);
            size_t total = strlen(text) - oldlen + newlen + 1;
            char *result = malloc(total);
            strncpy(result, text, pos - text);
            result[pos - text] = '\0';
            strcat(result, r->new);
            strcat(result, pos + oldlen);
            printf("%s\n", result);
            free(result);
            return;
        }
    }
    // no match, print original
    printf("%s\n", text);
}

void delete_rule(char *old) {
    Rule *prev = NULL;
    for (Rule *r = head; r; r = r->next) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->old);
            free(r->new);
            free(r);
            rule_count--;
            return;
        }
        prev = r;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    // get pointers to nodes and previous
    Rule *p_i = NULL, *p_j = NULL;
    Rule *n_i = NULL, *n_j = NULL;
    Rule *prev_i = NULL, *prev_j = NULL;
    Rule *cur = head;
    int idx = 0;
    while (cur) {
        if (idx == i) { p_i = cur; prev_i = prev_j; }
        if (idx == j) { p_j = cur; prev_j = prev_j; }
        prev_j = cur;
        cur = cur->next;
        idx++;
    }
    if (!p_i || !p_j) return;
    // swap by exchanging old and new strings (easier than relinking)
    char *tmp_old = p_i->old;
    char *tmp_new = p_i->new;
    p_i->old = p_j->old;
    p_i->new = p_j->new;
    p_j->old = tmp_old;
    p_j->new = tmp_new;
}

void rules_all(void) {
    for (Rule *r = head; r; r = r->next)
        printf("%s %s\n", r->old, r->new);
}

void free_all(void) {
    Rule *r = head;
    while (r) {
        Rule *next = r->next;
        free(r->old);
        free(r->new);
        free(r);
        r = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0 && n == 2) {
            apply_rule(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0 && n == 3) {
            swap_rules(atoi(arg1), atoi(arg2));
        } else if (strcmp(cmd, "RULES") == 0 && n == 1) {
            rules_all();
        }
    }
    free(line);
    free_all();
    return 0;
}