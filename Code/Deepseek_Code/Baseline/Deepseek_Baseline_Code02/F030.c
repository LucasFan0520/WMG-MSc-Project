// F030.c
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

Rule *get_rule(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *r = head;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = malloc(strlen(old) + 1);
    strcpy(r->old, old);
    r->new = malloc(strlen(new) + 1);
    strcpy(r->new, new);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
    rule_count++;
}

void apply_rules(const char *text) {
    Rule *r = head;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // replace first occurrence
            size_t prefix_len = pos - text;
            size_t old_len = strlen(r->old);
            size_t new_len = strlen(r->new);
            char *result = malloc(strlen(text) - old_len + new_len + 1);
            strncpy(result, text, prefix_len);
            result[prefix_len] = 0;
            strcat(result, r->new);
            strcat(result, pos + old_len);
            for (char *c = result; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            free(result);
            return;
        }
        r = r->next;
    }
    // no match, print unchanged with underscores
    for (char *c = text; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void delete_rule(const char *old) {
    Rule *prev = NULL;
    Rule *r = head;
    while (r) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (tail == r) tail = prev;
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
    if (i > j) { int tmp = i; i = j; j = tmp; }
    // get rules at i and j
    Rule *ri = get_rule(i);
    Rule *rj = get_rule(j);
    if (!ri || !rj) return;
    // swap old and new strings
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void list_rules(void) {
    Rule *r = head;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *old = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_rule(old, p);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            apply_rules(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_rule(p);
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            int i, j;
            if (sscanf(p, "%d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            list_rules();
        }
    }
    free_all();
    return 0;
}