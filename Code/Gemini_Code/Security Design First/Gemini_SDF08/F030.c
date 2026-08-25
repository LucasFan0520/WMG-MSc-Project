// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

int main(void) {
    Rule *rules = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *old_s = p;
                char *new_s = space + 1;
                Rule *tmp = realloc(rules, sizeof(Rule) * (count + 1));
                if (tmp) {
                    rules = tmp;
                    rules[count].old_str = strdup(old_s);
                    rules[count].new_str = strdup(new_s);
                    if (rules[count].old_str && rules[count].new_str) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            int matched = -1;
            char *pos = NULL;
            for (int i = 0; i < count; i++) {
                pos = strstr(text, rules[i].old_str);
                if (pos) {
                    matched = i;
                    break;
                }
            }
            if (matched != -1) {
                *pos = '\0';
                printf("%s%s%s\n", text, rules[matched].new_str, pos + strlen(rules[matched].old_str));
                *pos = rules[matched].old_str[0];
            } else {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_s = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(rules[i].old_str, old_s) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (int j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *i_str = p;
                char *j_str = space + 1;
                long idx_i = strtol(i_str, NULL, 10);
                long idx_j = strtol(j_str, NULL, 10);
                if (idx_i >= 0 && idx_i < count && idx_j >= 0 && idx_j < count) {
                    Rule tmp = rules[idx_i];
                    rules[idx_i] = rules[idx_j];
                    rules[idx_j] = tmp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    free(line);
    return 0;
}
