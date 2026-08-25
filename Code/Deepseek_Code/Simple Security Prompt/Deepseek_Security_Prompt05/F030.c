/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *rules = NULL;

Rule *find_rule(const char *old) {
    Rule *r = rules;
    while (r) {
        if (strcmp(r->old, old) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_rule(const char *old, const char *new) {
    if (find_rule(old)) return;
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
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
            size_t len = strlen(text) - strlen(r->old) + strlen(r->new) + 1;
            char *result = malloc(len);
            if (!result) return;
            char *p = result;
            const char *src = text;
            while (src < pos) *p++ = *src++;
            const char *n = r->new;
            while (*n) *p++ = *n++;
            src += strlen(r->old);
            while (*src) *p++ = *src++;
            *p = '\0';
            printf("%s\n", result);
            free(result);
            return;
        }
        r = r->next;
    }
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    Rule *r = rules, *prev = NULL;
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
    if (i == j) return;
    int count = 0;
    Rule *r = rules;
    while (r) { count++; r = r->next; }
    if (i < 0 || i >= count || j < 0 || j >= count) return;
    Rule *a = NULL, *b = NULL, *prev_a = NULL, *prev_b = NULL;
    Rule *cur = rules, *prev = NULL;
    int idx = 0;
    while (cur) {
        if (idx == i) { prev_a = prev; a = cur; }
        if (idx == j) { prev_b = prev; b = cur; }
        prev = cur;
        cur = cur->next;
        idx++;
    }
    if (!a || !b) return;
    if (prev_a) prev_a->next = b;
    else rules = b;
    if (prev_b) prev_b->next = a;
    else rules = a;
    Rule *tmp_next = a->next;
    a->next = b->next;
    b->next = tmp_next;
}

void rules_print(void) {
    Rule *r = rules;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0 && n >= 2) {
            char *txt = line + strlen(cmd) + 1;
            while (*txt == ' ') txt++;
            apply_rules(txt);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0 && n >= 3) {
            swap_rules(atoi(arg1), atoi(arg2));
        } else if (strcmp(cmd, "RULES") == 0) {
            rules_print();
        }
    }
    while (rules) {
        Rule *tmp = rules;
        rules = rules->next;
        free(tmp->old);
        free(tmp->new);
        free(tmp);
    }
    return 0;
}