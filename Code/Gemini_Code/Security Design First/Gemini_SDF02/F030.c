/* F030.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

int main(void) {
    Rule *rules = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *old_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *new_str = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Rule *temp = realloc(rules, new_cap * sizeof(Rule));
                if (!temp) break;
                rules = temp;
                capacity = new_cap;
            }
            rules[count].old_str = strdup(old_str);
            rules[count].new_str = strdup(new_str);
            if (rules[count].old_str && rules[count].new_str) count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = p;
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_str) == 0) {
                    printf("%s\n", rules[i].new_str);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_str = p;
            for (size_t i = 0; i < count; ) {
                if (strcmp(rules[i].old_str, old_str) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *idx1_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int idx1 = atoi(idx1_str);
            int idx2 = atoi(p);

            if (idx1 >= 0 && (size_t)idx1 < count && idx2 >= 0 && (size_t)idx2 < count) {
                Rule temp = rules[idx1];
                rules[idx1] = rules[idx2];
                rules[idx2] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}
