// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

int main() {
    Rule *rules = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *old_str = strtok(NULL, " ");
            char *new_str = strtok(NULL, " ");
            if (old_str && new_str) {
                rules = realloc(rules, (count + 1) * sizeof(Rule));
                rules[count].old_str = strdup(old_str);
                rules[count].new_str = strdup(new_str);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = strtok(NULL, " ");
            if (text) {
                int applied = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(text, rules[i].old_str) == 0) {
                        printf("%s\n", rules[i].new_str);
                        applied = 1;
                        break;
                    }
                }
                if (!applied) {
                    printf("%s\n", text);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_str = strtok(NULL, " ");
            if (old_str) {
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
                if (count == 0) {
                    free(rules);
                    rules = NULL;
                } else {
                    rules = realloc(rules, count * sizeof(Rule));
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_str = strtok(NULL, " ");
            char *j_str = strtok(NULL, " ");
            if (i_str && j_str) {
                int idxI = atoi(i_str);
                int idxJ = atoi(j_str);
                if (idxI >= 0 && idxI < count && idxJ >= 0 && idxJ < count) {
                    Rule temp = rules[idxI];
                    rules[idxI] = rules[idxJ];
                    rules[idxJ] = temp;
                }
            }
        } else if (strcmp(cmd, "RULES") == 0) {
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
    return 0;
}
