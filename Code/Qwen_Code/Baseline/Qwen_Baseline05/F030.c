// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 1000

typedef struct {
    char old_str[MAX_STR];
    char new_str[MAX_STR];
} Rule;

Rule rules[1000];
int rule_count = 0;

int main() {
    char cmd[20], old_str[MAX_STR], new_str[MAX_STR], text[MAX_STR];
    int i, j;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", old_str, new_str);
            strcpy(rules[rule_count].old_str, old_str);
            strcpy(rules[rule_count].new_str, new_str);
            rule_count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            scanf("%s", text);
            int applied = 0;
            for (i = 0; i < rule_count; i++) {
                if (strstr(text, rules[i].old_str) != NULL) {
                    char result[MAX_STR] = "";
                    char *pos = strstr(text, rules[i].old_str);
                    int len = pos - text;
                    strncpy(result, text, len);
                    result[len] = '\0';
                    strcat(result, rules[i].new_str);
                    strcat(result, pos + strlen(rules[i].old_str));
                    printf("%s\n", result);
                    applied = 1;
                    break;
                }
            }
            if (!applied) printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", old_str);
            for (i = 0; i < rule_count; i++) {
                if (strcmp(rules[i].old_str, old_str) == 0) {
                    for (j = i; j < rule_count - 1; j++) rules[j] = rules[j + 1];
                    rule_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            scanf("%d %d", &i, &j);
            if (i >= 0 && i < rule_count && j >= 0 && j < rule_count) {
                Rule temp = rules[i];
                rules[i] = rules[j];
                rules[j] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (i = 0; i < rule_count; i++) printf("%s %s\n", rules[i].old_str, rules[i].new_str);
        }
    }
    return 0;
}