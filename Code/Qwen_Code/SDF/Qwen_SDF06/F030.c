// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old_str;
    char *new_str;
    struct Rule *next;
} Rule;

Rule *head = NULL;
int rule_count = 0;

Rule *get_rule(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    return curr;
}

void add_rule(const char *old_str, const char *new_str) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old_str = strdup(old_str);
    r->new_str = strdup(new_str);
    if (!r->old_str || !r->new_str) { free(r->old_str); free(r->new_str); free(r); return; }
    r->next = head;
    head = r;
    rule_count++;
}

void apply_rule(const char *text) {
    for (Rule *r = head; r; r = r->next) {
        if (strcmp(text, r->old_str) == 0) {
            printf("%s\n", r->new_str);
            return;
        }
    }
    printf("%s\n", text);
}

void delete_rule(const char *old_str) {
    Rule *curr = head;
    Rule *prev = NULL;
    while (curr) {
        if (strcmp(curr->old_str, old_str) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->old_str);
            free(curr->new_str);
            free(curr);
            rule_count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    Rule *ri = get_rule(i);
    Rule *rj = get_rule(j);
    char *temp_old = ri->old_str;
    char *temp_new = ri->new_str;
    ri->old_str = rj->old_str;
    ri->new_str = rj->new_str;
    rj->old_str = temp_old;
    rj->new_str = temp_new;
}

void print_rules() {
    for (Rule *r = head; r; r = r->next) {
        printf("%s %s\n", r->old_str, r->new_str);
    }
}

void free_all() {
    Rule *r = head;
    while (r) {
        Rule *next = r->next;
        free(r->old_str);
        free(r->new_str);
        free(r);
        r = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        int i = 0, j = 0;
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0 && n >= 2) {
            apply_rule(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %d %d", &i, &j) == 2) {
                swap_rules(i, j);
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_all();
    return 0;
}