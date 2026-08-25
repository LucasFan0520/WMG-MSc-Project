// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULES 1000
#define MAX_LEN 1000

typedef struct {
    char old_str[100];
    char new_str[100];
} Rule;

Rule rules[MAX_RULES];
int rcount = 0;

int find_rule(const char *old_str) {
    for (int i = 0; i < rcount; i++) if (strcmp(rules[i].old_str, old_str) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], old_str[100], new_str[100], text[MAX_LEN];
    int i, j;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", old_str, new_str);
            if (find_rule(old_str) == -1 && rcount < MAX_RULES) {
                strcpy(rules[rcount].old_str, old_str);
                strcpy(rules[rcount].new_str, new_str);
                rcount++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            int matched = 0;
            for (int k = 0; k < rcount; k++) {
                char *pos = strstr(text, rules[k].old_str);
                if (pos != NULL) {
                    char temp[MAX_LEN];
                    int prefix_len = pos - text;
                    strncpy(temp, text, prefix_len);
                    temp[prefix_len] = '\0';
                    strcat(temp, rules[k].new_str);
                    strcat(temp, pos + strlen(rules[k].old_str));
                    printf("%s\n", temp);
                    matched = 1;
                    break;
                }
            }
            if (!matched) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            scanf("%d %d", &i, &j);
            if (i >= 0 && i < rcount && j >= 0 && j < rcount) {
                Rule temp = rules[i];
                rules[i] = rules[j];
                rules[j] = temp;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", old_str);
            int idx = find_rule(old_str);
            if (idx != -1) {
                for (int k = idx; k < rcount - 1; k++) rules[k] = rules[k + 1];
                rcount--;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int k = 0; k < rcount; k++) {
                printf("%s %s\n", rules[k].old_str, rules[k].new_str);
            }
        }
    }
    return 0;
}