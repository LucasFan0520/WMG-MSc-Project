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
    Rule *r = head;
    for (int i = 0; r && i < index; i++) r = r->next;
    return r;
}

void free_rule(Rule *r) {
    free(r->old);
    free(r->new);
    free(r);
}

void handle_add(char *cmd) {
    char *old = strtok(NULL, " ");
    if (!old) return;
    char *new = strtok(NULL, "");
    if (!new) new = "";
    Rule *newrule = malloc(sizeof(Rule));
    newrule->old = strdup(old);
    newrule->new = strdup(new);
    newrule->next = NULL;
    if (!head) head = newrule;
    else {
        Rule *last = head;
        while (last->next) last = last->next;
        last->next = newrule;
    }
    rule_count++;
}

void handle_apply(char *cmd) {
    char *text = strtok(NULL, "");
    if (!text) return;
    char *result = NULL;
    Rule *r = head;
    while (r) {
        char *pos = strstr(text, r->old);
        if (pos) {
            // replace first occurrence
            size_t len = strlen(text) + strlen(r->new) - strlen(r->old) + 1;
            result = malloc(len);
            size_t prefix = pos - text;
            strncpy(result, text, prefix);
            result[prefix] = '\0';
            strcat(result, r->new);
            strcat(result, pos + strlen(r->old));
            break;
        }
        r = r->next;
    }
    if (!result) {
        result = strdup(text);
    }
    for (char *s = result; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
    free(result);
}

void handle_delete(char *cmd) {
    char *old = strtok(NULL, " ");
    if (!old) return;
    Rule *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_rule(r);
            rule_count--;
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_swap(char *cmd) {
    char *istr = strtok(NULL, " ");
    char *jstr = strtok(NULL, " ");
    if (!istr || !jstr) return;
    int i = atoi(istr);
    int j = atoi(jstr);
    if (i < 0 || j < 0 || i >= rule_count || j >= rule_count || i == j) return;
    Rule *ri = get_rule(i);
    Rule *rj = get_rule(j);
    if (!ri || !rj) return;
    // Swap old and new strings
    char *tmp_old = ri->old;
    char *tmp_new = ri->new;
    ri->old = rj->old;
    ri->new = rj->new;
    rj->old = tmp_old;
    rj->new = tmp_new;
}

void handle_rules() {
    Rule *r = head;
    int idx = 0;
    while (r) {
        printf("%d %s %s\n", idx, r->old, r->new);
        r = r->next;
        idx++;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "APPLY") == 0) handle_apply(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "SWAP") == 0) handle_swap(cmd);
        else if (strcmp(cmd, "RULES") == 0) handle_rules();
    }
    while (head) {
        Rule *tmp = head;
        head = head->next;
        free_rule(tmp);
    }
    return 0;
}