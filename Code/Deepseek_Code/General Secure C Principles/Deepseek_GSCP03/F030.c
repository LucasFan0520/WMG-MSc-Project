/* F030.c */
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

Rule *get_rule_at(int index) {
    if (index < 0 || index >= rule_count) return NULL;
    Rule *r = head;
    for (int i = 0; i < index; i++) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old = strdup(old);
    r->new = strdup(new);
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
            // replace first occurrence
            size_t prefix_len = pos - text;
            size_t old_len = strlen(r->old);
            size_t new_len = strlen(r->new);
            size_t suffix_len = strlen(pos + old_len);
            char *result = malloc(prefix_len + new_len + suffix_len + 1);
            if (!result) {
                printf("%s\n", text);
                return;
            }
            strncpy(result, text, prefix_len);
            strcpy(result + prefix_len, r->new);
            strcpy(result + prefix_len + new_len, pos + old_len);
            printf("%s\n", result);
            free(result);
            return;
        }
        r = r->next;
    }
    printf("%s\n", text);
}

void delete_rule(const char *old) {
    Rule *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->old, old) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->old);
            free(curr->new);
            free(curr);
            rule_count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) return;
    // get nodes
    Rule *prev_i = NULL, *node_i = head;
    for (int idx = 0; idx < i; idx++) {
        prev_i = node_i;
        node_i = node_i->next;
    }
    Rule *prev_j = NULL, *node_j = head;
    for (int idx = 0; idx < j; idx++) {
        prev_j = node_j;
        node_j = node_j->next;
    }
    // swap
    Rule *tmp_next = node_i->next;
    if (prev_i) prev_i->next = node_j;
    else head = node_j;
    if (prev_j) prev_j->next = node_i;
    else head = node_i;
    // swap next pointers correctly
    Rule *temp = node_i->next;
    node_i->next = node_j->next;
    node_j->next = temp;
    // update tail if needed
    if (tail == node_i) tail = node_j;
    else if (tail == node_j) tail = node_i;
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
        Rule *tmp = r;
        r = r->next;
        free(tmp->old);
        free(tmp->new);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char old[256], new[256];
            if (sscanf(line + 4, "%255s %255s", old, new) == 2) {
                add_rule(old, new);
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            apply_rule(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_rule(p);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(line + 5, "%d %d", &i, &j) == 2) {
                swap_rules(i, j);
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            list_rules();
        }
    }
    free_all();
    return 0;
}