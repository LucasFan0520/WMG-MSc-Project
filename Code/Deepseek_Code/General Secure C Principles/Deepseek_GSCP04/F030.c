/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
} Rule;

static Rule *rules = NULL;
static size_t rcount = 0, rcap = 0;

static void add_rule(const char *old, const char *new) {
    if (rcount == rcap) {
        rcap = rcap ? rcap * 2 : 4;
        rules = realloc(rules, rcap * sizeof(Rule));
        if (!rules) { perror("realloc"); exit(1); }
    }
    rules[rcount].old = strdup(old);
    rules[rcount].new = strdup(new);
    rcount++;
}

static void delete_rule(const char *old) {
    for (size_t i = 0; i < rcount; i++) {
        if (strcmp(rules[i].old, old) == 0) {
            free(rules[i].old);
            free(rules[i].new);
            for (size_t j = i; j < rcount - 1; j++)
                rules[j] = rules[j + 1];
            rcount--;
            return;
        }
    }
}

static void swap_rules(size_t i, size_t j) {
    if (i >= rcount || j >= rcount) return;
    Rule tmp = rules[i];
    rules[i] = rules[j];
    rules[j] = tmp;
}

static void apply_text(const char *text) {
    for (size_t i = 0; i < rcount; i++) {
        const char *pos = strstr(text, rules[i].old);
        if (pos) {
            // replace first occurrence
            size_t oldlen = strlen(rules[i].old);
            size_t newlen = strlen(rules[i].new);
            size_t prefix = pos - text;
            size_t suffix = strlen(text) - prefix - oldlen;
            char *result = malloc(prefix + newlen + suffix + 1);
            if (!result) { perror("malloc"); exit(1); }
            memcpy(result, text, prefix);
            memcpy(result + prefix, rules[i].new, newlen);
            memcpy(result + prefix + newlen, pos + oldlen, suffix);
            result[prefix + newlen + suffix] = '\0';
            printf("%s\n", result);
            free(result);
            return;
        }
    }
    // no match
    printf("%s\n", text);
}

static void print_rules(void) {
    for (size_t i = 0; i < rcount; i++) {
        printf("%s %s\n", rules[i].old, rules[i].new);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *old = strtok(NULL, " ");
            if (!old) continue;
            char *new = strtok(NULL, " ");
            if (!new) new = "";
            add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = strtok(NULL, "");
            if (!text) text = "";
            apply_text(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = strtok(NULL, " ");
            if (old) delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i1 = strtok(NULL, " ");
            char *i2 = strtok(NULL, " ");
            if (i1 && i2) {
                int a = atoi(i1), b = atoi(i2);
                if (a >= 0 && b >= 0) swap_rules((size_t)a, (size_t)b);
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    for (size_t i = 0; i < rcount; i++) {
        free(rules[i].old);
        free(rules[i].new);
    }
    free(rules);
    return 0;
}