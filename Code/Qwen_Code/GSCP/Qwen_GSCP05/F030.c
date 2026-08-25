// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_name;
    char *new_name;
} Rule;

Rule *rules = NULL;
int rulec = 0;

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[512];
        char a2[512];
        a1[0] = '\0';
        a2[0] = '\0';
        int n = sscanf(line, "%31s %511s %511[^\n]", cmd, a1, a2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            rules = realloc(rules, (rulec + 1) * sizeof(Rule));
            if (!rules) return 1;
            rules[rulec].old_name = strdup(a1);
            rules[rulec].new_name = strdup(a2);
            rulec++;
        } else if (strcmp(cmd, "APPLY") == 0 && n >= 2) {
            char *text = a1;
            int found = 0;
            for (int i = 0; i < rulec; i++) {
                char *pos = strstr(text, rules[i].old_name);
                if (pos != NULL) {
                    size_t prefix_len = (size_t)(pos - text);
                    size_t suffix_len = strlen(pos + strlen(rules[i].old_name));
                    size_t new_len = prefix_len + strlen(rules[i].new_name) + suffix_len;
                    char *result = malloc(new_len + 1);
                    if (!result) return 1;
                    memcpy(result, text, prefix_len);
                    memcpy(result + prefix_len, rules[i].new_name, strlen(rules[i].new_name));
                    memcpy(result + prefix_len + strlen(rules[i].new_name), pos + strlen(rules[i].old_name), suffix_len);
                    result[new_len] = '\0';
                    printf("%s\n", result);
                    free(result);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            for (int i = 0; i < rulec; i++) {
                if (strcmp(rules[i].old_name, a1) == 0) {
                    free(rules[i].old_name);
                    free(rules[i].new_name);
                    for (int k = i; k < rulec - 1; k++) {
                        rules[k] = rules[k + 1];
                    }
                    rulec--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            n = sscanf(line, "%31s %d %d", cmd, &i, &j);
            if (n >= 3) {
                if (i >= 0 && i < rulec && j >= 0 && j < rulec) {
                    Rule tmp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = tmp;
                }
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < rulec; i++) {
                printf("%s %s\n", rules[i].old_name, rules[i].new_name);
            }
        }
    }
    for (int i = 0; i < rulec; i++) {
        free(rules[i].old_name);
        free(rules[i].new_name);
    }
    free(rules);
    return 0;
}