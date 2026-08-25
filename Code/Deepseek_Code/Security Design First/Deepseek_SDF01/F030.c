// filename: F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

static Rule *rules = NULL;

static void cleanup(void) {
    while (rules) {
        Rule *tmp = rules;
        rules = rules->next;
        free(tmp->old);
        free(tmp->new);
        free(tmp);
    }
}

static void add_rule(const char *old, const char *new) {
    Rule *r = malloc(sizeof(Rule));
    if (!r) return;
    r->old = strdup(old);
    r->new = strdup(new);
    if (!r->old || !r->new) {
        free(r->old);
        free(r->new);
        free(r);
        return;
    }
    r->next = rules;
    rules = r;
}

static void delete_rule(const char *old) {
    Rule *prev = NULL;
    for (Rule *r = rules; r; r = r->next) {
        if (strcmp(r->old, old) == 0) {
            if (prev) prev->next = r->next;
            else rules = r->next;
            free(r->old);
            free(r->new);
            free(r);
            return;
        }
        prev = r;
    }
}

static void swap_rules(int i, int j) {
    if (i < 0 || j < 0) return;
    // convert to array to swap
    int count = 0;
    for (Rule *r = rules; r; r = r->next) ++count;
    if (i >= count || j >= count) return;
    Rule **arr = malloc(count * sizeof(Rule *));
    if (!arr) return;
    int idx = 0;
    for (Rule *r = rules; r; r = r->next) arr[idx++] = r;
    Rule *tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    // rebuild list
    rules = arr[0];
    for (int k = 0; k < count - 1; ++k) arr[k]->next = arr[k+1];
    arr[count-1]->next = NULL;
    free(arr);
}

static void apply_rule(const char *text) {
    for (Rule *r = rules; r; r = r->next) {
        if (strcmp(r->old, text) == 0) {
            printf("%s\n", r->new);
            return;
        }
    }
    printf("%s\n", text);
}

static void print_rules(void) {
    for (Rule *r = rules; r; r = r->next)
        printf("%s %s\n", r->old, r->new);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1) add_rule(arg1, arg2);
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (*arg1) apply_rule(arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_rule(arg1);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (*arg1 && *arg2) swap_rules(atoi(arg1), atoi(arg2));
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free(line);
    return 0;
}