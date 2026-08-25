// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old_str;
    char *new_str;
    struct Rule *next;
} Rule;

Rule *rules = NULL;
int rule_count = 0;

void add_rule(const char *old_str, const char *new_str) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) exit(EXIT_FAILURE);
    r->old_str = strdup(old_str);
    r->new_str = strdup(new_str);
    r->next = NULL;
    if (!rules) {
        rules = r;
    } else {
        Rule *curr = rules;
        while (curr->next) curr = curr->next;
        curr->next = r;
    }
    rule_count++;
}

void apply_rule(const char *text) {
    Rule *curr = rules;
    while (curr) {
        char *pos = strstr(text, curr->old_str);
        if (pos) {
            size_t old_len = strlen(curr->old_str);
            size_t new_len = strlen(curr->new_str);
            size_t text_len = strlen(text);
            size_t new_text_len = text_len - old_len + new_len;
            char *new_text = malloc(new_text_len + 1);
            if (!new_text) exit(EXIT_FAILURE);
            
            size_t prefix_len = pos - text;
            strncpy(new_text, text, prefix_len);
            strcpy(new_text + prefix_len, curr->new_str);
            strcpy(new_text + prefix_len + new_len, pos + old_len);
            
            printf("%s\n", new_text);
            free(new_text);
            return;
        }
        curr = curr->next;
    }
    printf("%s\n", text);
}

void delete_rule(const char *old_str) {
    Rule *curr = rules;
    Rule *prev = NULL;
    while (curr) {
        if (strcmp(curr->old_str, old_str) == 0) {
            if (prev) prev->next = curr->next;
            else rules = curr->next;
            free(curr->old_str);
            free(curr->new_str);
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
    Rule *curr_i = rules;
    Rule *curr_j = rules;
    for (int k = 0; k < i; k++) curr_i = curr_i->next;
    for (int k = 0; k < j; k++) curr_j = curr_j->next;
    
    char *temp = curr_i->old_str;
    curr_i->old_str = curr_j->old_str;
    curr_j->old_str = temp;
    
    temp = curr_i->new_str;
    curr_i->new_str = curr_j->new_str;
    curr_j->new_str = temp;
}

void print_rules() {
    Rule *curr = rules;
    while (curr) {
        printf("%s %s\n", curr->old_str, curr->new_str);
        curr = curr->next;
    }
}

void free_all() {
    Rule *curr = rules;
    while (curr) {
        Rule *next = curr->next;
        free(curr->old_str);
        free(curr->new_str);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[3500] = {0};
        int idx1 = 0, idx2 = 0;
        
        int matched = sscanf(line, "%31s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (matched >= 2) sscanf(arg1, "%d", &idx1);
        if (matched >= 3) sscanf(arg2, "%d", &idx2);
        
        if (strcmp(cmd, "ADD") == 0 && matched >= 2) {
            add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0) {
            apply_rule(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            swap_rules(idx1, idx2);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_all();
    return 0;
}