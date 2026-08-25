/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *rules = NULL;
int rule_count = 0;

Rule *get_rule(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *r = rules;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void free_rule(Rule *r) {
    free(r->old);
    free(r->new);
    free(r);
}

void add_rule(const char *old, const char *new) {
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
            size_t len = strlen(r->old);
            size_t newlen = strlen(r->new);
            size_t total = strlen(text) - len + newlen + 1;
            char *result = malloc(total);
            strncpy(result, text, pos - text);
            result[pos - text] = '\0';
            strcat(result, r->new);
            strcat(result, pos + len);
            for (char *c = result; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            free(result);
            return;
        }
        r = r->next;
    }
    for (char *c = text; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void delete_rule(const char *old) {
    Rule *prev = NULL, *curr = rules;
    while (curr) {
        if (strcmp(curr->old, old) == 0) {
            if (prev) prev->next = curr->next;
            else rules = curr->next;
            free_rule(curr);
            rule_count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    if (i > j) { int tmp = i; i = j; j = tmp; }
    Rule *prev_i = NULL, *node_i = rules;
    for (int k = 0; k < i; k++) { prev_i = node_i; node_i = node_i->next; }
    Rule *prev_j = NULL, *node_j = rules;
    for (int k = 0; k < j; k++) { prev_j = node_j; node_j = node_j->next; }
    Rule *next_i = node_i->next;
    Rule *next_j = node_j->next;
    if (prev_i) prev_i->next = node_j;
    else rules = node_j;
    node_j->next = next_i;
    if (prev_j != node_i) {
        prev_j->next = node_i;
        node_i->next = next_j;
    } else {
        node_i->next = node_j;
        node_j->next = next_i;
    }
}

void print_rules(void) {
    Rule *r = rules;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char old[256], new[4096], text[4096];
    int i, j;

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", old) != 1) continue;
            p += strlen(old);
            while (*p == ' ') p++;
            add_rule(old, p);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            apply_rule(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", old) == 1) delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }

    while (rules) {
        Rule *tmp = rules;
        rules = rules->next;
        free_rule(tmp);
    }
    return 0;
}