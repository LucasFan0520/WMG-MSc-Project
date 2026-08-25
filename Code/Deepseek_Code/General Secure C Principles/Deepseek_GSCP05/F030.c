// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *rules = NULL;

int rule_count(void) {
    int cnt = 0;
    Rule *r = rules;
    while (r) { cnt++; r = r->next; }
    return cnt;
}

Rule *get_rule_at(int index) {
    int i = 0;
    Rule *r = rules;
    while (r) {
        if (i == index) return r;
        i++;
        r = r->next;
    }
    return NULL;
}

void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old = strdup(old);
    r->new = strdup(new);
    r->next = rules;
    rules = r;
}

void apply_rule(const char *text) {
    Rule *r = rules;
    while (r) {
        if (strcmp(r->old, text) == 0) {
            printf("%s\n", r->new);
            return;
        }
        r = r->next;
    }
    // no match, print original
    printf("%s\n", text);
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
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void swap_rules(int i, int j) {
    int cnt = rule_count();
    if (i < 0 || i >= cnt || j < 0 || j >= cnt || i == j) return;
    // collect rules in array for swap
    Rule **arr = malloc(cnt * sizeof(Rule *));
    if (!arr) return;
    Rule *r = rules;
    for (int k = 0; k < cnt; k++) {
        arr[k] = r;
        r = r->next;
    }
    // swap pointers
    Rule *tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    // rebuild list
    for (int k = 0; k < cnt - 1; k++) {
        arr[k]->next = arr[k+1];
    }
    arr[cnt-1]->next = NULL;
    rules = arr[0];
    free(arr);
}

void print_rules(void) {
    Rule *r = rules;
    while (r) {
        printf("%s -> %s\n", r->old, r->new);
        r = r->next;
    }
}

void free_all(void) {
    Rule *r = rules;
    while (r) {
        Rule *next = r->next;
        free(r->old);
        free(r->new);
        free(r);
        r = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *old = p;
            char *space = strchr(old, ' ');
            if (!space) continue;
            *space = '\0';
            char *new = space + 1;
            while (*new == ' ') new++;
            add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            apply_rule(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *old = p;
            char *space = strchr(old, ' ');
            if (space) *space = '\0';
            delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%d %d", &i, &j) == 2) swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free_all();
    return 0;
}