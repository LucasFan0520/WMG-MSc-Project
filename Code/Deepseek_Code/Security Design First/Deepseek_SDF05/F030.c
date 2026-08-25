// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *rules = NULL;
int rule_count = 0;

Rule *get_rule_at(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *cur = rules;
    for (int i = 0; i < index; i++) cur = cur->next;
    return cur;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old = safe_strdup(old);
    r->new = safe_strdup(new);
    r->next = rules;
    rules = r;
    rule_count++;
}

void apply_text(const char *text) {
    Rule *cur = rules;
    int matched = 0;
    while (cur) {
        if (strstr(text, cur->old)) {
            // replace first occurrence
            const char *pos = strstr(text, cur->old);
            size_t before = pos - text;
            size_t old_len = strlen(cur->old);
            size_t new_len = strlen(cur->new);
            size_t total_len = before + new_len + strlen(pos + old_len) + 1;
            char *result = malloc(total_len);
            if (!result) return;
            snprintf(result, total_len, "%.*s%s%s", (int)before, text, cur->new, pos + old_len);
            printf("%s\n", result);
            free(result);
            matched = 1;
            break;
        }
        cur = cur->next;
    }
    if (!matched) printf("%s\n", text);
}

void delete_rule(const char *old) {
    Rule *prev = NULL, *cur = rules;
    while (cur) {
        if (strcmp(cur->old, old) == 0) {
            if (prev) prev->next = cur->next;
            else rules = cur->next;
            free(cur->old);
            free(cur->new);
            free(cur);
            rule_count--;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    // We need to swap by index. We'll convert to array for simplicity.
    Rule **arr = malloc(rule_count * sizeof(Rule *));
    if (!arr) return;
    Rule *cur = rules;
    for (int k = 0; k < rule_count; k++) {
        arr[k] = cur;
        cur = cur->next;
    }
    Rule *tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    // Rebuild list
    rules = arr[0];
    for (int k = 0; k < rule_count - 1; k++) {
        arr[k]->next = arr[k+1];
    }
    arr[rule_count-1]->next = NULL;
    free(arr);
}

void rules_print(void) {
    Rule *cur = rules;
    while (cur) {
        printf("%s %s\n", cur->old, cur->new);
        cur = cur->next;
    }
}

void free_all(void) {
    Rule *cur = rules;
    while (cur) {
        Rule *next = cur->next;
        free(cur->old);
        free(cur->new);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            apply_text(p);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0 && n >= 3) {
            int i = atoi(arg1);
            int j = atoi(arg2);
            swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            rules_print();
        }
    }
    free_all();
    return 0;
}