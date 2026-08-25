// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

Rule *rules = NULL;
int rcount = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char old_s[100], new_s[100];
            scanf("%s %s", old_s, new_s);
            rules = realloc(rules, sizeof(Rule) * (rcount + 1));
            rules[rcount].old_str = strdup(old_s);
            rules[rcount].new_str = strdup(new_s);
            rcount++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char text[1000];
            scanf(" %[^\n]", text);
            int found = 0;
            for (int i = 0; i < rcount; i++) {
                char *pos = strstr(text, rules[i].old_str);
                if (pos) {
                    char out[2000] = "";
                    int len = strlen(rules[i].old_str);
                    strncpy(out, text, pos - text);
                    out[pos - text] = '\0';
                    strcat(out, rules[i].new_str);
                    strcat(out, pos + len);
                    printf("%s\n", out);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old_s[100];
            scanf("%s", old_s);
            for (int i = 0; i < rcount; i++) {
                if (strcmp(rules[i].old_str, old_s) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (int j = i; j < rcount - 1; j++) rules[j] = rules[j+1];
                    rcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            scanf("%d %d", &i, &j);
            if (i >= 0 && i < rcount && j >= 0 && j < rcount) {
                Rule temp = rules[i];
                rules[i] = rules[j];
                rules[j] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}