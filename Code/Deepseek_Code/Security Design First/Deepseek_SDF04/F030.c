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

Rule *get_rule_at(int index) {
    Rule *r = head;
    while (r && index--) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = NULL;
    if (!r->old || !r->new) {
        free(r->old); free(r->new); free(r);
        return;
    }
    if (!head) head = r;
    else {
        Rule *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
    rule_count++;
}

void apply_rules(const char *text) {
    Rule *r = head;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // simple replace first occurrence
            size_t newlen = strlen(text) - strlen(r->old) + strlen(r->new) + 1;
            char *result = malloc(newlen);
            if (!result) return;
            char *out = result;
            size_t before = pos - text;
            strncpy(out, text, before);
            out += before;
            strcpy(out, r->new);
            out += strlen(r->new);
            strcpy(out, pos + strlen(r->old));
            printf("%s\n", result);
            free(result);
            return;
        }
        r = r->next;
    }
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
    int n = rule_count;
    if (i < 0 || i >= n || j < 0 || j >= n || i == j) return;
    Rule *ri = get_rule_at(i);
    Rule *rj = get_rule_at(j);
    if (!ri || !rj) return;
    // swap contents
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void print_rules(void) {
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], old[256], new[4096], text[4096];
        int i, j;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", old, new) == 2)
                add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", text) == 1)
                apply_rules(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", old) == 1)
                delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %d %d", &i, &j) == 2)
                swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free(line);
    free_all();
    return 0;
}