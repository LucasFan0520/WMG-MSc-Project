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

int rule_count() {
    int c = 0;
    Rule *r = head;
    while (r) { c++; r = r->next; }
    return c;
}

Rule* get_rule_at(int index) {
    int c = 0;
    Rule *r = head;
    while (r) {
        if (c == index) return r;
        c++;
        r = r->next;
    }
    return NULL;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = head;
    head = r;
}

void apply_rule(const char *text) {
    Rule *r = head;
    char *result = strdup(text);
    while (r) {
        char *pos = strstr(result, r->old);
        if (pos) {
            char *new_res = malloc(strlen(result) - strlen(r->old) + strlen(r->new) + 1);
            strncpy(new_res, result, pos - result);
            new_res[pos - result] = '\0';
            strcat(new_res, r->new);
            strcat(new_res, pos + strlen(r->old));
            free(result);
            result = new_res;
            break;
        }
        r = r->next;
    }
    printf("%s\n", result);
    free(result);
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
            return;
        }
        prev = r;
        r = r->next;
    }
}

void swap_rules(int i, int j) {
    int len = rule_count();
    if (i < 0 || i >= len || j < 0 || j >= len) return;
    if (i == j) return;
    Rule *ri = get_rule_at(i);
    Rule *rj = get_rule_at(j);
    char *temp_old = ri->old;
    char *temp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = temp_old;
    rj->new = temp_new;
}

void rules_print() {
    Rule *r = head;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

void free_all() {
    while (head) {
        Rule *tmp = head;
        head = head->next;
        free(tmp->old);
        free(tmp->new);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, a, b);
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_rule(a, b);
        } else if (strcmp(cmd, "APPLY") == 0 && n >= 2) {
            apply_rule(a);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_rule(a);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(line, "%*s %d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0 && n == 1) {
            rules_print();
        }
    }
    free_all();
    return 0;
}