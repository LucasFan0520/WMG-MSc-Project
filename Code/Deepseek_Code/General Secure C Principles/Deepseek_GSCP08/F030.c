// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule { char *old; char *new; struct Rule *next; } Rule;
Rule *rules = NULL;

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = rules;
    rules = r;
}

void apply_rule(const char *text) {
    for (Rule *r = rules; r; r = r->next) {
        if (strcmp(r->old, text) == 0) {
            for (char *c = r->new; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            return;
        }
    }
    // no match: print original text with spaces as underscores
    for (const char *c = text; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void delete_rule(const char *old) {
    Rule *prev = NULL;
    for (Rule *r = rules; r; prev = r, r = r->next) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next; else rules = r->next;
            free(r->old); free(r->new); free(r);
            return;
        }
    }
}

void swap_rules(int i, int j) {
    // convert to array
    int count = 0;
    for (Rule *r = rules; r; r = r->next) count++;
    if (i < 0 || i >= count || j < 0 || j >= count || i == j) return;
    Rule **arr = malloc(count * sizeof(Rule *));
    if (!arr) return;
    int idx = 0;
    for (Rule *r = rules; r; r = r->next) arr[idx++] = r;
    // swap pointers
    Rule *tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    // rebuild list
    rules = NULL;
    for (int k = count - 1; k >= 0; k--) {
        arr[k]->next = rules;
        rules = arr[k];
    }
    free(arr);
}

void print_rules(void) {
    for (Rule *r = rules; r; r = r->next)
        printf("%s %s\n", r->old, r->new);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char old[256], new[4096];
            if (sscanf(rest, "%255s", old) == 1) {
                char *n = rest + strlen(old);
                while (*n == ' ') n++;
                strcpy(new, n);
                add_rule(old, new);
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char text[4096];
            strcpy(text, rest);
            apply_rule(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old[256];
            if (sscanf(rest, "%255s", old) == 1) delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(rest, "%d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    while (rules) { Rule *r = rules; rules = r->next; free(r->old); free(r->new); free(r); }
    return 0;
}