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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char old[256], new_s[256];
            if (sscanf(args, "%255s %255s", old, new_s) == 2) {
                rules = realloc(rules, (count + 1) * sizeof(Rule));
                rules[count].old_str = strdup(old);
                rules[count].new_str = strdup(new_s);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char text[256];
            if (sscanf(args, "%255s", text) == 1) {
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
            char old[256];
            if (sscanf(args, "%255s", old) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(rules[i].old_str, old) == 0) {
                        free(rules[i].old_str);
                        free(rules[i].new_str);
                        for (int j = i; j < count - 1; j++) {
                            rules[j] = rules[j + 1];
                        }
                        count--;
                        rules = realloc(rules, count * sizeof(Rule));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i = 0, j = 0;
            if (sscanf(args, "%d %d", &i, &j) == 2) {
                if (i >= 0 && i < count && j >= 0 && j < count) {
                    Rule temp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = temp;
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
