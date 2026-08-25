// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_val;
    char *new_val;
} Rule;

int main(void) {
    Rule *rules = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *old_val = args;
                char *new_val = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Rule *next = realloc(rules, capacity * sizeof(Rule));
                    if (!next) break;
                    rules = next;
                }
                rules[count].old_val = strdup(old_val);
                rules[count].new_val = strdup(new_val);
                count++;
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_val) == 0) {
                    printf("%s\n", rules[i].new_val);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_val = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rules[i].old_val, old_val) == 0) {
                    free(rules[i].old_val);
                    free(rules[i].new_val);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                int i = atoi(args);
                int j = atoi(space + 1);
                if (i >= 0 && (size_t)i < count && j >= 0 && (size_t)j < count && i != j) {
                    Rule temp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = temp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", rules[i].old_val, rules[i].new_val);
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
