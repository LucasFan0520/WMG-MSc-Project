/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rule {
    char *old;
    char *new;
    struct rule *next;
} rule_t;

rule_t *head = NULL;
rule_t *tail = NULL;
int rule_count = 0;

rule_t *find_rule_old(const char *old) {
    rule_t *r = head;
    while (r) {
        if (strcmp(r->old, old) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_rule(const char *old, const char *new) {
    if (find_rule_old(old)) return;
    rule_t *r = malloc(sizeof(rule_t));
    if (!r) return;
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = NULL;
    if (!r->old || !r->new) {
        free(r->old); free(r->new); free(r);
        return;
    }
    if (tail) tail->next = r;
    else head = r;
    tail = r;
    rule_count++;
}

void apply_rule(const char *text) {
    rule_t *r = head;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // replace first occurrence
            size_t oldlen = strlen(r->old);
            size_t newlen = strlen(r->new);
            size_t prefix = pos - text;
            size_t suffix = strlen(text) - prefix - oldlen;
            char *result = malloc(prefix + newlen + suffix + 1);
            if (!result) return;
            memcpy(result, text, prefix);
            memcpy(result + prefix, r->new, newlen);
            memcpy(result + prefix + newlen, pos + oldlen, suffix + 1);
            // print with spaces as underscores
            for (char *s = result; *s; s++) {
                if (*s == ' ') putchar('_');
                else putchar(*s);
            }
            putchar('\n');
            free(result);
            return;
        }
        r = r->next;
    }
    // no match: print text as is with underscores
    for (char *s = (char*)text; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void delete_rule(const char *old) {
    rule_t *prev = NULL, *r = head;
    while (r) {
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
        r = r->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    // get pointers to nodes at indices i and j
    rule_t *a = head, *b = head;
    for (int k = 0; k < i; k++) a = a->next;
    for (int k = 0; k < j; k++) b = b->next;
    // swap old and new strings
    char *tmp_old = a->old;
    char *tmp_new = a->new;
    a->old = b->old;
    a->new = b->new;
    b->old = tmp_old;
    b->new = tmp_new;
}

void rules_list(void) {
    rule_t *r = head;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

void free_all(void) {
    rule_t *r = head;
    while (r) {
        rule_t *nxt = r->next;
        free(r->old);
        free(r->new);
        free(r);
        r = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        if (sscanf(line, "%9s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                add_rule(a, rest);
            } else if (strcmp(cmd, "APPLY") == 0) {
                apply_rule(rest);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_rule(a);
            } else if (strcmp(cmd, "SWAP") == 0) {
                int i, j;
                if (sscanf(line, "%*s %d %d", &i, &j) == 2)
                    swap_rules(i, j);
            } else if (strcmp(cmd, "RULES") == 0) {
                rules_list();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}