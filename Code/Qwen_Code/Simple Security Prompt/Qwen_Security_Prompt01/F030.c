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

void add_rule(const char *o, const char *n) {
    Rule *r = malloc(sizeof(Rule));
    r->old_str = strdup(o);
    r->new_str = strdup(n);
    r->next = NULL;
    if (!head) head = r;
    else {
        Rule *p = head;
        while (p->next) p = p->next;
        p->next = r;
    }
    rule_count++;
}

void apply_rule(const char *text) {
    for (Rule *r = head; r; r = r->next) {
        char *pos = strstr(text, r->old_str);
        if (pos) {
            size_t len = strlen(text) - strlen(r->old_str) + strlen(r->new_str) + 1;
            char *buf = malloc(len);
            size_t prefix_len = pos - text;
            strncpy(buf, text, prefix_len);
            buf[prefix_len] = 0;
            strcat(buf, r->new_str);
            strcat(buf, pos + strlen(r->old_str));
            printf("%s\n", buf);
            free(buf);
            return;
        }
    }
    printf("%s\n", text);
}

void delete_rule(const char *o) {
    Rule *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->old_str, o) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->old_str); free(p->new_str); free(p);
            rule_count--;
            return;
        }
        prev = p; p = p->next;
    }
}

void swap_rule(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    Rule *pi = head, *pj = head;
    Rule *prev_i = NULL, *prev_j = NULL;
    for (int k = 0; k < i; k++) { prev_i = pi; pi = pi->next; }
    for (int k = 0; k < j; k++) { prev_j = pj; pj = pj->next; }
    
    if (prev_i) prev_i->next = pj; else head = pj;
    if (prev_j) prev_j->next = pi; else head = pi;
    
    Rule *tmp = pi->next;
    pi->next = pj->next;
    pj->next = tmp;
}

void print_rules() {
    for (Rule *r = head; r; r = r->next) {
        printf("%s %s\n", r->old_str, r->new_str);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000];
        int i, j;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            add_rule(a, b);
        } else if (strcmp(cmd, "APPLY") == 0) {
            sscanf(line, "%*s %[^\n]", a);
            apply_rule(a);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_rule(a);
        } else if (strcmp(cmd, "SWAP") == 0) {
            sscanf(line, "%*s %d %d", &i, &j);
            swap_rule(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    while (head) {
        Rule *n = head->next;
        free(head->old_str); free(head->new_str); free(head);
        head = n;
    }
    return 0;
}