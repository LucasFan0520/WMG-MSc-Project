// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_val;
    char *new_val;
} RenameRule;

int main(void) {
    RenameRule *rules = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *new_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*old == '\0' || *new_str == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                RenameRule *new_rules = realloc(rules, new_cap * sizeof(RenameRule));
                if (!new_rules) continue;
                rules = new_rules;
                capacity = new_cap;
            }
            char *o = strdup(old);
            char *n = strdup(new_str);
            if (o && n) {
                rules[count].old_val = o;
                rules[count].new_val = n;
                count++;
            } else {
                free(o);
                free(n);
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                char *pos = strstr(text, rules[i].old_val);
                if (pos) {
                    size_t old_len = strlen(rules[i].old_val);
                    size_t new_len = strlen(rules[i].new_val);
                    size_t prefix_len = pos - text;
                    size_t suffix_len = strlen(pos + old_len);
                    char *res = malloc(prefix_len + new_len + suffix_len + 1);
                    if (res) {
                        memcpy(res, text, prefix_len);
                        memcpy(res + prefix_len, rules[i].new_val, new_len);
                        strcpy(res + prefix_len + new_len, pos + old_len);
                        printf("%s\n", res);
                        free(res);
                    }
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*old == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(rules[i].old_val, old) == 0) {
                    free(rules[i].old_val);
                    free(rules[i].new_val);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *i_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *j_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*i_str == '\0' || *j_str == '\0') continue;
            long idx_i = strtol(i_str, NULL, 10);
            long idx_j = strtol(j_str, NULL, 10);
            if (idx_i < 0 || (size_t)idx_i >= count || idx_j < 0 || (size_t)idx_j >= count) continue;

            if (idx_i != idx_j) {
                RenameRule tmp = rules[idx_i];
                rules[idx_i] = rules[idx_j];
                rules[idx_j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%zu: %s -> %s\n", i, rules[i].old_val, rules[i].new_val);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_val);
        free(rules[i].new_val);
    }
    free(rules);
    free(line);
    return 0;
}
