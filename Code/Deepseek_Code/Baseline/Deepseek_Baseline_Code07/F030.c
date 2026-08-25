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
    Rule *r = head;
    int i = 0;
    while (r) {
        if (i == index) return r;
        r = r->next;
        i++;
    }
    return NULL;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = malloc(strlen(old)+1);
    strcpy(r->old, old);
    r->new = malloc(strlen(new)+1);
    strcpy(r->new, new);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
    rule_count++;
}

void apply_rule(const char *text) {
    Rule *r = head;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            size_t prefix_len = pos - text;
            size_t suffix_len = strlen(text) - (pos - text) - strlen(r->old);
            char *result = malloc(prefix_len + strlen(r->new) + suffix_len + 1);
            strncpy(result, text, prefix_len);
            result[prefix_len] = 0;
            strcat(result, r->new);
            strcat(result, pos + strlen(r->old));
            printf("%s\n", result);
            free(result);
            return;
        }
        r = r->next;
    }
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    Rule *prev = NULL;
    Rule *r = head;
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
    Rule *ri = get_rule(i);
    Rule *rj = get_rule(j);
    if (!ri || !rj) return;
    // swap old and new
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *old = strtok(rest, " ");
            if (!old) continue;
            char *new = strtok(NULL, "");
            if (!new) new = "";
            add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = rest;
            apply_rule(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = strtok(rest, " ");
            if (old) delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_str = strtok(rest, " ");
            char *j_str = strtok(NULL, " ");
            if (i_str && j_str) swap_rules(atoi(i_str), atoi(j_str));
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_all();
    return 0;
}