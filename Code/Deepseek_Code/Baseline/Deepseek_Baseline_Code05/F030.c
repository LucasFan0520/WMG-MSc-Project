/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rule {
    char *old;
    char *new;
    struct rule *next;
} Rule;

Rule *head = NULL;
int rule_count = 0;

Rule *find_rule_by_old(const char *old) {
    Rule *r = head;
    while (r) {
        if (strcmp(r->old, old) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_rule(const char *old, const char *new) {
    if (find_rule_by_old(old)) return;
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = head;
    head = r;
    rule_count++;
}

void apply_rule(const char *text) {
    Rule *r = head;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            char result[1024];
            size_t before = pos - text;
            size_t after = strlen(pos + strlen(r->old));
            snprintf(result, sizeof(result), "%.*s%s%.*s", (int)before, text, r->new, (int)after, pos + strlen(r->old));
            for (char *c = result; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            return;
        }
        r = r->next;
    }
    // no match: print original with underscores
    for (char *c = text; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void delete_rule(const char *old) {
    Rule *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
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
    if (i < 0 || j < 0 || i >= rule_count || j >= rule_count) return;
    if (i == j) return;
    Rule *ri = NULL, *rj = NULL, *prev_i = NULL, *prev_j = NULL;
    Rule *cur = head, *prev = NULL;
    int idx = 0;
    while (cur) {
        if (idx == i) { ri = cur; prev_i = prev; }
        if (idx == j) { rj = cur; prev_j = prev; }
        prev = cur;
        cur = cur->next;
        idx++;
    }
    if (!ri || !rj) return;
    // swap nodes
    Rule *tmp_next = rj->next;
    if (prev_i) prev_i->next = rj;
    else head = rj;
    if (prev_j) prev_j->next = ri;
    else head = ri;
    Rule *tmp = ri->next;
    ri->next = rj->next;
    rj->next = tmp;
    // fix if they were adjacent
    if (ri->next == ri) ri->next = rj;
    if (rj->next == rj) rj->next = ri;
}

void rules_print() {
    Rule *r = head;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], old[256], new[256], text[1024];
        int i, j;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "RULES") == 0) { rules_print(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s", old, new) == 2)
                add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *rest = line + strlen(cmd) + 1;
            if (*rest == ' ') rest++;
            apply_rule(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", old) == 1) delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %d %d", &i, &j) == 2) swap_rules(i, j);
        }
    }
    return 0;
}