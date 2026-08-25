// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
} Rule;

Rule *rules = NULL;
int rule_count = 0;
int rule_capacity = 0;

void add_rule(const char *old, const char *new) {
    if (rule_count == rule_capacity) {
        rule_capacity = rule_capacity ? rule_capacity * 2 : 4;
        rules = realloc(rules, rule_capacity * sizeof(Rule));
    }
    rules[rule_count].old = malloc(strlen(old) + 1);
    strcpy(rules[rule_count].old, old);
    rules[rule_count].new = malloc(strlen(new) + 1);
    strcpy(rules[rule_count].new, new);
    rule_count++;
}

void delete_rule(const char *old) {
    for (int i = 0; i < rule_count; i++) {
        if (strcmp(rules[i].old, old) == 0) {
            free(rules[i].old);
            free(rules[i].new);
            for (int j = i; j < rule_count - 1; j++) {
                rules[j] = rules[j + 1];
            }
            rule_count--;
            return;
        }
    }
}

void apply_rule(const char *text) {
    for (int i = 0; i < rule_count; i++) {
        char *pos = strstr(text, rules[i].old);
        if (pos) {
            char *result = malloc(strlen(text) + strlen(rules[i].new) + 1);
            char *p = result;
            while (text < pos) *p++ = *text++;
            text = pos;
            for (char *q = rules[i].new; *q; q++) *p++ = *q;
            text += strlen(rules[i].old);
            while (*text) *p++ = *text++;
            *p = '\0';
            printf("%s\n", result);
            free(result);
            return;
        }
    }
    printf("%s\n", text);
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= rule_count || j < 0 || j >= rule_count) return;
    Rule tmp = rules[i];
    rules[i] = rules[j];
    rules[j] = tmp;
}

void print_rules(void) {
    for (int i = 0; i < rule_count; i++) {
        printf("%s %s\n", rules[i].old, rules[i].new);
    }
}

void free_all(void) {
    for (int i = 0; i < rule_count; i++) {
        free(rules[i].old);
        free(rules[i].new);
    }
    free(rules);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *old = strtok(NULL, " ");
            char *new = strtok(NULL, " ");
            if (old && new) add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = strtok(NULL, "");
            if (text) apply_rule(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = strtok(NULL, " ");
            if (old) delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i = strtok(NULL, " ");
            char *j = strtok(NULL, " ");
            if (i && j) swap_rules(atoi(i), atoi(j));
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free(line);
    free_all();
    return 0;
}