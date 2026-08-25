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

Rule* get_rule(int index) {
    int i = 0;
    Rule *cur = head;
    while (cur && i < index) { cur = cur->next; i++; }
    return (i == index) ? cur : NULL;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = NULL;
    if (!head) head = r;
    else {
        Rule *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = r;
    }
    rule_count++;
}

void delete_rule(const char *old) {
    Rule *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->old, old) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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
    if (i == j || i < 0 || j < 0 || i >= rule_count || j >= rule_count) return;
    Rule *ri = get_rule(i);
    Rule *rj = get_rule(j);
    if (!ri || !rj) return;
    // swap data
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void apply_rule(const char *text) {
    Rule *cur = head;
    while (cur) {
        char *pos = strstr(text, cur->old);
        if (pos) {
            // replace first occurrence
            size_t prefix_len = pos - text;
            size_t old_len = strlen(cur->old);
            size_t new_len = strlen(cur->new);
            size_t total_len = strlen(text) - old_len + new_len + 1;
            char *result = malloc(total_len);
            strncpy(result, text, prefix_len);
            result[prefix_len] = 0;
            strcat(result, cur->new);
            strcat(result, pos + old_len);
            printf("%s\n", result);
            free(result);
            return;
        }
        cur = cur->next;
    }
    // no match
    printf("%s\n", text);
}

void print_rules() {
    Rule *cur = head;
    while (cur) {
        printf("%s %s\n", cur->old, cur->new);
        cur = cur->next;
    }
}

void free_all() {
    Rule *cur = head;
    while (cur) {
        Rule *tmp = cur;
        cur = cur->next;
        free(tmp->old);
        free(tmp->new);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *old = strtok(NULL, " ");
            if (!old) continue;
            char *new = strtok(NULL, "");
            if (!new) new = "";
            add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = strtok(NULL, "");
            if (!text) text = "";
            apply_rule(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = strtok(NULL, " ");
            if (old) delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_str = strtok(NULL, " ");
            if (!i_str) continue;
            char *j_str = strtok(NULL, " ");
            if (!j_str) continue;
            int i = atoi(i_str);
            int j = atoi(j_str);
            swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_all();
    return 0;
}