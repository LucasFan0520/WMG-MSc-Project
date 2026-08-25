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

void free_list() {
    while (head) {
        Rule *r = head; head = head->next;
        free(r->old); free(r->new); free(r);
    }
    rule_count = 0;
}

Rule *get_rule(int index) {
    Rule *r = head;
    for (int i = 0; i < index && r; i++) r = r->next;
    return r;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], old_str[256], new_str[256], text[4096] = "";
        int i, j;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s", old_str, new_str) != 2) continue;
            Rule *r = malloc(sizeof(Rule));
            r->old = strdup(old_str); r->new = strdup(new_str);
            r->next = head; head = r;
            rule_count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", text) != 1) continue;
            int found = 0;
            for (Rule *r = head; r; r = r->next) {
                char *pos = strstr(text, r->old);
                if (pos) {
                    char res[8192];
                    int len = strlen(r->old);
                    strncpy(res, text, pos - text);
                    res[pos - text] = 0;
                    strcat(res, r->new);
                    strcat(res, pos + len);
                    printf("%s\n", res);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", old_str) != 1) continue;
            Rule *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->old, old_str) == 0) {
                    Rule *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->old); free(tmp->new); free(tmp);
                    rule_count--;
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %d %d", &i, &j) != 2) continue;
            if (i < 0 || i >= rule_count || j < 0 || j >= rule_count || i == j) continue;
            Rule *ri = get_rule(i), *rj = get_rule(j);
            char *tmp = ri->old; ri->old = rj->old; rj->old = tmp;
            tmp = ri->new; ri->new = rj->new; rj->new = tmp;
        } else if (strcmp(cmd, "RULES") == 0) {
            for (Rule *r = head; r; r = r->next) {
                printf("%s %s\n", r->old, r->new);
            }
        }
    }
    free_list();
    return 0;
}