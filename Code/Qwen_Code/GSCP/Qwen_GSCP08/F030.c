// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old_str;
    char *new_str;
    struct Rule *next;
} Rule;

Rule *head = NULL;
int rule_count = 0;

void add_rule(char *old_str, char *new_str) {
    Rule *r = malloc(sizeof(Rule));
    r->old_str = strdup(old_str);
    r->new_str = strdup(new_str);
    r->next = NULL;
    if (!head) {
        head = r;
    } else {
        Rule *last = head;
        while (last->next) last = last->next;
        last->next = r;
    }
    rule_count++;
}

void apply_rule(char *text) {
    for (Rule *r = head; r; r = r->next) {
        char *pos = strstr(text, r->old_str);
        if (pos) {
            size_t old_len = strlen(r->old_str);
            size_t new_len = strlen(r->new_str);
            size_t text_len = strlen(text);
            char *new_text = malloc(text_len - old_len + new_len + 1);
            strncpy(new_text, text, pos - text);
            new_text[pos - text] = '\0';
            strcat(new_text, r->new_str);
            strcat(new_text, pos + old_len);
            printf("%s\n", new_text);
            free(new_text);
            return;
        }
    }
    printf("%s\n", text);
}

void delete_rule(char *old_str) {
    Rule *prev = NULL;
    Rule *curr = head;
    while (curr) {
        if (strcmp(curr->old_str, old_str) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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
    Rule *prev_i = NULL, *curr_i = head;
    for (int k = 0; k < i; k++) { prev_i = curr_i; curr_i = curr_i->next; }
    Rule *prev_j = NULL, *curr_j = head;
    for (int k = 0; k < j; k++) { prev_j = curr_j; curr_j = curr_j->next; }
    
    if (prev_i) prev_i->next = curr_j; else head = curr_j;
    if (prev_j) prev_j->next = curr_i; else head = curr_i;
    
    Rule *temp_next = curr_i->next;
    curr_i->next = curr_j->next;
    curr_j->next = temp_next;
}

void print_rules() {
    for (Rule *r = head; r; r = r->next) {
        printf("%s %s\n", r->old_str, r->new_str);
    }
}

int main() {
    char cmd[20], a[100], b[1000];
    int i, j;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s%99s", a, b);
            add_rule(a, b);
        } else if (strcmp(cmd, "APPLY") == 0) {
            fgets(b, sizeof(b), stdin);
            b[strcspn(b, "\n")] = 0;
            apply_rule(b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_rule(a);
        } else if (strcmp(cmd, "SWAP") == 0) {
            scanf("%d%d", &i, &j);
            swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    return 0;
}