// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} RenameRule;

RenameRule *rules = NULL;
int count = 0;
int capacity = 0;

void add_rule(const char *old_str, const char *new_str) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        rules = realloc(rules, capacity * sizeof(RenameRule));
    }
    rules[count].old_str = strdup(old_str);
    rules[count].new_str = strdup(new_str);
    count++;
}

void apply_rules(const char *text) {
    int matched_idx = -1;
    char *pos = NULL;
    for (int i = 0; i < count; i++) {
        pos = strstr(text, rules[i].old_str);
        if (pos != NULL) {
            matched_idx = i;
            break;
        }
    }
    if (matched_idx == -1) {
        printf("%s\n", text);
    } else {
        size_t text_len = strlen(text);
        size_t old_len = strlen(rules[matched_idx].old_str);
        size_t new_len = strlen(rules[matched_idx].new_str);
        size_t prefix_len = pos - text;
        size_t suffix_len = text_len - prefix_len - old_len;
        char *result = malloc(prefix_len + new_len + suffix_len + 1);
        strncpy(result, text, prefix_len);
        strcpy(result + prefix_len, rules[matched_idx].new_str);
        strcpy(result + prefix_len + new_len, pos + old_len);
        printf("%s\n", result);
        free(result);
    }
}

void delete_rule(const char *old_str) {
    for (int i = 0; i < count; i++) {
        if (strcmp(rules[i].old_str, old_str) == 0) {
            free(rules[i].old_str);
            free(rules[i].new_str);
            for (int j = i; j < count - 1; j++) {
                rules[j] = rules[j + 1];
            }
            count--;
            i--;
        }
    }
}

void swap_rules(int i, int j) {
    if (i < 0 || i >= count || j < 0 || j >= count) return;
    RenameRule temp = rules[i];
    rules[i] = rules[j];
    rules[j] = temp;
}

void print_rules(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", rules[i].old_str, rules[i].new_str);
    }
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *old_str = strtok(NULL, " ");
            char *new_str = strtok(NULL, "");
            if (old_str && new_str) add_rule(old_str, new_str);
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = strtok(NULL, "");
            if (text) apply_rules(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_str = strtok(NULL, "");
            if (old_str) delete_rule(old_str);
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_s = strtok(NULL, " ");
            char *j_s = strtok(NULL, "");
            if (i_s && j_s) swap_rules(atoi(i_s), atoi(j_s));
        } else if (strcmp(cmd, "RULES") == 0) {
            print_rules();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}
