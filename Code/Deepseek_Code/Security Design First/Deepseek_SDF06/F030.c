// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
} Rule;

static Rule *rules = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void add_rule(const char *old, const char *new) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Rule *new_r = realloc(rules, capacity * sizeof(Rule));
        if (!new_r) { fprintf(stderr, "Memory error\n"); exit(1); }
        rules = new_r;
    }
    rules[count].old = strdup(old);
    rules[count].new = strdup(new);
    if (!rules[count].old || !rules[count].new) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void delete_rule(const char *old) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(rules[i].old, old) == 0) {
            free(rules[i].old);
            free(rules[i].new);
        } else {
            if (write != i) rules[write] = rules[i];
            write++;
        }
    }
    count = write;
}

static void swap_rules(size_t i, size_t j) {
    if (i >= count || j >= count) return;
    Rule tmp = rules[i];
    rules[i] = rules[j];
    rules[j] = tmp;
}

static void apply_rule(const char *text) {
    for (size_t i = 0; i < count; i++) {
        char *pos = strstr(text, rules[i].old);
        if (pos) {
            // Replace first occurrence
            size_t old_len = strlen(rules[i].old);
            size_t new_len = strlen(rules[i].new);
            size_t text_len = strlen(text);
            size_t result_len = text_len - old_len + new_len + 1;
            char *result = malloc(result_len);
            if (!result) { fprintf(stderr, "Memory error\n"); exit(1); }
            size_t before = pos - text;
            strncpy(result, text, before);
            result[before] = '\0';
            strcat(result, rules[i].new);
            strcat(result, pos + old_len);
            printf("%s\n", result);
            free(result);
            return;
        }
    }
    printf("%s\n", text);
}

static void rules_print(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", rules[i].old, rules[i].new);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old);
        free(rules[i].new);
    }
    free(rules);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], old[256], new[4096];
    size_t i, j;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", old, new) == 2)
                add_rule(old, new);
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", new) == 1)
                apply_rule(new);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", old) == 1)
                delete_rule(old);
        } else if (strcmp(cmd, "SWAP") == 0) {
            if (sscanf(line, "%*s %zu %zu", &i, &j) == 2)
                swap_rules(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            rules_print();
        }
    }

    free(line);
    free_all();
    return 0;
}