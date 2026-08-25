// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *old; char *new; } Rule;
Rule rules[10000]; int nrules = 0;

int main() {
    char cmd[20], old_str[100], new_str[100], text[1000]; int i, j;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", old_str);
            fgets(new_str, sizeof(new_str), stdin);
            new_str[strcspn(new_str, "\n")] = 0;
            if (new_str[0] == ' ') memmove(new_str, new_str + 1, strlen(new_str));
            rules[nrules].old = strdup(old_str);
            rules[nrules++].new = strdup(new_str);
        } else if (!strcmp(cmd, "APPLY")) {
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            int found = 0;
            for (i = 0; i < nrules; i++) {
                char *pos = strstr(text, rules[i].old);
                if (pos) {
                    char res[2000] = "";
                    strncpy(res, text, pos - text);
                    res[pos - text] = 0;
                    strcat(res, rules[i].new);
                    strcat(res, pos + strlen(rules[i].old));
                    printf("%s\n", res);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("%s\n", text);
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", old_str);
            for (i = 0; i < nrules; i++) {
                if (!strcmp(rules[i].old, old_str)) {
                    free(rules[i].old);
                    free(rules[i].new);
                    for (j = i; j < nrules - 1; j++) rules[j] = rules[j + 1];
                    nrules--;
                    break;
                }
            }
        } else if (!strcmp(cmd, "SWAP")) {
            scanf("%d %d", &i, &j);
            if (i >= 0 && i < nrules && j >= 0 && j < nrules) {
                Rule t = rules[i];
                rules[i] = rules[j];
                rules[j] = t;
            }
        } else if (!strcmp(cmd, "RULES")) {
            for (i = 0; i < nrules; i++) {
                printf("%s %s\n", rules[i].old, rules[i].new);
            }
        }
    }
    for (i = 0; i < nrules; i++) {
        free(rules[i].old);
        free(rules[i].new);
    }
    return 0;
}