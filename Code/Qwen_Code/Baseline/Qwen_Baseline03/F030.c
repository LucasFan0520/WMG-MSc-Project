// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char old[100];
    char new[100];
} Rule;

Rule rules[1000];
int rn = 0;

int main() {
    char cmd[20], old[100], new[100], text[1000];
    int i, j;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s", old, new);
            strcpy(rules[rn].old, old);
            strcpy(rules[rn].new, new);
            rn++;
        } else if (!strcmp(cmd, "APPLY")) {
            fgets(text, 1000, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            for (i = 0; i < rn; i++) {
                char *pos = strstr(text, rules[i].old);
                if (pos) {
                    char res[2000] = "";
                    int len = strlen(rules[i].old);
                    strncat(res, text, pos - text);
                    strcat(res, rules[i].new);
                    strcat(res, pos + len);
                    printf("%s\n", res);
                    break;
                }
            }
            if (i == rn) {
                printf("%s\n", text);
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", old);
            for (i = 0; i < rn; i++) {
                if (!strcmp(rules[i].old, old)) {
                    for (j = i; j < rn - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    rn--;
                    break;
                }
            }
        } else if (!strcmp(cmd, "SWAP")) {
            scanf("%d %d", &i, &j);
            if (i >= 0 && i < rn && j >= 0 && j < rn) {
                Rule tmp = rules[i];
                rules[i] = rules[j];
                rules[j] = tmp;
            }
        } else if (!strcmp(cmd, "RULES")) {
            for (i = 0; i < rn; i++) {
                printf("%s %s\n", rules[i].old, rules[i].new);
            }
        }
    }
    return 0;
}