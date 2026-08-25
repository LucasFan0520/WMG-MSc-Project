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
    r->next = head;
    head = r;
    rule_count++;
}

void apply_rule(const char *text) {
    Rule *r = head;
    while (r) {
        if (strstr(text, r->old)) {
            // simple replacement: find first occurrence
            const char *pos = strstr(text, r->old);
            size_t before = pos - text;
            size_t oldlen = strlen(r->old);
            size_t newlen = strlen(r->new);
            size_t after = strlen(text) - before - oldlen;
            char *result = malloc(before + newlen + after + 1);
            strncpy(result, text, before);
            strcpy(result + before, r->new);
            strcpy(result + before + newlen, pos + oldlen);
            printf("%s\n", result);
            free(result);
            return;
        }
        r = r->next;
    }
    // no match, print original
    printf("%s\n", text);
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
    if (i == j || i < 0 || i >= rule_count || j < 0 || j >= rule_count) return;
    Rule *ri = get_rule(i);
    Rule *rj = get_rule(j);
    if (!ri || !rj) return;
    // swap data without changing pointers? easier: swap the old/new strings
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void rules_print(void) {
    Rule *r = head;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

void free_all(void) {
    Rule *r = head;
    while (r) {
        Rule *tmp = r->next;
        free(r->old);
        free(r->new);
        free(r);
        r = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[4096];
        int idx, idx2;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s", a) == 1) {
                const char *n = rest + strlen(a) + 1;
                while (*n == ' ') n++;
                if (*n) add_rule(a, n);
                else add_rule(a, "");
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            apply_rule(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", a) == 1) delete_rule(a);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(rest, "%d %d", &idx, &idx2) == 2) swap_rules(idx, idx2);
        } else if (strcmp(cmd, "RULES") == 0) {
            rules_print();
        }
    }
    free_all();
    return 0;
}