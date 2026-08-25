// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rule {
    char *old;
    char *new;
    struct rule *next;
} Rule;

Rule *head = NULL;
Rule *tail = NULL;
int rule_count = 0;

Rule *get_rule(int index) {
    if (index < 0) return NULL;
    Rule *r = head;
    for (int i = 0; i < index && r; i++) r = r->next;
    return r;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
    rule_count++;
}

void apply_rules(const char *text) {
    char buffer[4096];
    strcpy(buffer, text);
    Rule *r = head;
    while (r) {
        char *pos = strstr(buffer, r->old);
        if (pos) {
            char result[4096];
            int len = pos - buffer;
            strncpy(result, buffer, len);
            result[len] = '\0';
            strcat(result, r->new);
            strcat(result, pos + strlen(r->old));
            strcpy(buffer, result);
            break;
        }
        r = r->next;
    }
    for (char *c = buffer; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void delete_rule(const char *old) {
    Rule *r = head, *prev = NULL;
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
    if (i < 0 || j < 0 || i >= rule_count || j >= rule_count || i == j) return;
    Rule *a = get_rule(i);
    Rule *b = get_rule(j);
    if (!a || !b) return;
    char *tmp_old = a->old;
    char *tmp_new = a->new;
    a->old = b->old;
    a->new = b->new;
    b->old = tmp_old;
    b->new = tmp_new;
}

void rules_print(void) {
    Rule *r = head;
    while (r) {
        printf("%s %s\n", r->old, r->new);
        r = r->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        int i, j;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (sscanf(line, "%*s %[^\n]", arg1) == 1)
                apply_rules(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %d %d", &i, &j) == 2)
                swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            rules_print();
        }
    }
    return 0;
}