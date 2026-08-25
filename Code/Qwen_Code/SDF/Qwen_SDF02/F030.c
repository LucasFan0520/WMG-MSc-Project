// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Rule {
    char *old_str;
    char *new_str;
} Rule;

Rule *rules = NULL;
size_t count = 0, capacity = 0;

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char old_s[256], new_s[256];
            if (sscanf(rest, "%255s %255s", old_s, new_s) != 2) continue;
            if (count == capacity) {
                size_t new_cap = capacity ? capacity * 2 : 4;
                Rule *tmp = realloc(rules, new_cap * sizeof(Rule));
                if (!tmp) continue;
                rules = tmp;
                capacity = new_cap;
            }
            rules[count].old_str = strdup(old_s);
            rules[count].new_str = strdup(new_s);
            if (!rules[count].old_str || !rules[count].new_str) {
                free(rules[count].old_str); free(rules[count].new_str); continue;
            }
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char text[4096];
            if (sscanf(rest, "%4095s", text) != 1) continue;
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_str) == 0) {
                    printf("%s\n", rules[i].new_str);
                    applied = 1;
                    break;
                }
            }
            if (!applied) printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old_s[256];
            if (sscanf(rest, "%255s", old_s) != 1) continue;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rules[i].old_str, old_s) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (size_t j = i; j < count - 1; j++) rules[j] = rules[j+1];
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            if (sscanf(rest, "%d %d", &i, &j) != 2) continue;
            if (i < 0 || (size_t)i >= count || j < 0 || (size_t)j >= count) continue;
            Rule tmp = rules[i];
            rules[i] = rules[j];
            rules[j] = tmp;
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}