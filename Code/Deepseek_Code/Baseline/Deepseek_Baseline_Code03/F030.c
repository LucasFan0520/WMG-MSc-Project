// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rule {
    char *old;
    char *new;
    struct rule *next;
};

struct rule *rules = NULL;
int rule_count = 0;

struct rule *get_rule_by_index(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    struct rule *r = rules;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void add_rule(char *old, char *new) {
    struct rule *r = malloc(sizeof(struct rule));
    r->old = malloc(strlen(old) + 1);
    strcpy(r->old, old);
    r->new = malloc(strlen(new) + 1);
    strcpy(r->new, new);
    r->next = rules;
    rules = r;
    rule_count++;
}

void delete_rule(char *old) {
    struct rule *r = rules, *prev = NULL;
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

void apply_rule(char *text) {
    struct rule *r = rules;
    while (r) {
        if (strcmp(r->old, text) == 0) {
            printf("%s\n", r->new);
            return;
        }
        r = r->next;
    }
    printf("%s\n", text);
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    // We need to swap by index; since it's a singly linked list, we need to get nodes and their predecessors.
    // We'll implement by extracting nodes and reinserting.
    // Simpler: swap the data (old, new) between nodes.
    struct rule *ri = get_rule_by_index(i);
    struct rule *rj = get_rule_by_index(j);
    if (!ri || !rj) return;
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void print_rules() {
    struct rule *r = rules;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        int i, j;
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0 && n >= 2) {
            char *rest = line + strlen(cmd) + 1;
            apply_rule(rest);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%31s %d %d", cmd, &i, &j) >= 3) {
                swap_rules(i, j);
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    return 0;
}