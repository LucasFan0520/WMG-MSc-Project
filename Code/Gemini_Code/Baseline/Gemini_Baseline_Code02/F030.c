// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *old_pattern;
    char *new_pattern;
} Rule;

int main() {
    Rule *rules = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "RULES") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s -> %s\n", rules[i].old_pattern, rules[i].new_pattern);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                rules = realloc(rules, sizeof(Rule) * (count + 1));
                rules[count].old_pattern = strdup(args);
                rules[count].new_pattern = strdup(p2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            int rule_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(rules[i].old_pattern, args) == 0) {
                    rule_idx = i;
                    break;
                }
            }
            if (rule_idx != -1) {
                printf("%s\n", rules[rule_idx].new_pattern);
            } else {
                printf("%s\n", args);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(rules[i].old_pattern, args) == 0) {
                    free(rules[i].old_pattern);
                    free(rules[i].new_pattern);
                    for (int j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    if (count == 0) { free(rules); rules = NULL; }
                    else { rules = realloc(rules, sizeof(Rule) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int i = atoi(args);
                int j = atoi(p2 + 1);
                if (i >= 0 && i < count && j >= 0 && j < count) {
                    Rule tmp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = tmp;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(rules[i].old_pattern);
        free(rules[i].new_pattern);
    }
    free(rules);
    return 0;
}
