// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

Rule *rules = NULL;
int num_rules = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ADD") == 0) {
            char old_s[256], new_s[256];
            sscanf(line, "%*s %s %s", old_s, new_s);
            rules = realloc(rules, (num_rules + 1) * sizeof(Rule));
            rules[num_rules].old_str = strdup(old_s);
            rules[num_rules].new_str = strdup(new_s);
            num_rules++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char text[4096];
            char *p = strchr(line, ' ');
            if (p) {
                p++;
                strcpy(text, p);
            } else {
                text[0] = 0;
            }
            int applied = 0;
            for (int i = 0; i < num_rules; i++) {
                char *pos = strstr(text, rules[i].old_str);
                if (pos) {
                    char result[4096];
                    int len_old = strlen(rules[i].old_str);
                    int len_new = strlen(rules[i].new_str);
                    int prefix_len = pos - text;
                    strncpy(result, text, prefix_len);
                    result[prefix_len] = 0;
                    strcat(result, rules[i].new_str);
                    strcat(result, pos + len_old);
                    strcpy(text, result);
                    applied = 1;
                    break;
                }
            }
            printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old_s[256];
            sscanf(line, "%*s %s", old_s);
            for (int i = 0; i < num_rules; i++) {
                if (strcmp(rules[i].old_str, old_s) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (int j = i; j < num_rules - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    num_rules--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            sscanf(line, "%*s %d %d", &i, &j);
            if (i >= 0 && i < num_rules && j >= 0 && j < num_rules) {
                Rule tmp = rules[i];
                rules[i] = rules[j];
                rules[j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < num_rules; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    
    for (int i = 0; i < num_rules; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    
    return 0;
}