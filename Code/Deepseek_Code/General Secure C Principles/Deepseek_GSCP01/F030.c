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
int rule_count = 0;

Rule *get_rule_at(int index) {
    Rule *r = rules;
    int i = 0;
    while (r && i < index) {
        r = r->next;
        i++;
    }
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = rules;
    rules = r;
    rule_count++;
}

void apply_rules(const char *text) {
    Rule *r = rules;
    while (r) {
        if (strstr(text, r->old) == text) {
            printf("%s", r->new);
            printf("%s", text + strlen(r->old));
            putchar('\n');
            return;
        }
        r = r->next;
    }
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    Rule *r = rules, *prev = NULL;
    while (r && strcmp(r->old, old) != 0) {
        prev = r;
        r = r->next;
    }
    if (!r) return;
    if (prev) prev->next = r->next;
    else rules = r->next;
    free(r->old);
    free(r->new);
    free(r);
    rule_count--;
}

void swap_rules(int i, int j) {
    if (i < 0 || j < 0 || i >= rule_count || j >= rule_count || i == j) return;
    Rule *ri = get_rule_at(i);
    Rule *rj = get_rule_at(j);
    if (!ri || !rj) return;
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void print_rules(void) {
    Rule *r = rules;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

void free_all(void) {
    Rule *r = rules;
    while (r) {
        Rule *next = r->next;
        free(r->old);
        free(r->new);
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char old[256], new[256];
            if (sscanf(line + 4, "%255s %255s", old, new) == 2)
                add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            apply_rules(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_rule(p);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(line + 5, "%d %d", &i, &j) == 2)
                swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_all();
    free(line);
    return 0;
}