// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_rule;
    char *new_rule;
} RenameRule;

int main(void) {
    RenameRule *rules = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *old_r = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *new_r = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            RenameRule *temp = realloc(rules, (count + 1) * sizeof(RenameRule));
            if (temp) {
                rules = temp;
                rules[count].old_rule = strdup(old_r);
                rules[count].new_rule = strdup(new_r);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *text = p;
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_rule) == 0) {
                    printf("%s\n", rules[i].new_rule);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *old_r = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(rules[i].old_rule, old_r) == 0) {
                    free(rules[i].old_rule);
                    free(rules[i].new_rule);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *i_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *j_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int idx_i = atoi(i_s);
            int idx_j = atoi(j_s);
            if (idx_i >= 0 && (size_t)idx_i < count && idx_j >= 0 && (size_t)idx_j < count) {
                RenameRule tmp = rules[idx_i];
                rules[idx_i] = rules[idx_j];
                rules[idx_j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_rule, rules[i].new_rule);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_rule);
        free(rules[i].new_rule);
    }
    free(rules);
    free(line);
    return 0;
}
