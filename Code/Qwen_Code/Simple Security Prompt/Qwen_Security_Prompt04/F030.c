// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old;
    char *new_str;
} Rule;

Rule rules[1000];
int rc = 0;

int find_rule(char *old) {
    for (int i = 0; i < rc; i++) if (strcmp(rules[i].old, old) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char old[1000], new_str[1000];
            sscanf(rest, "%s %s", old, new_str);
            if (find_rule(old) == -1) {
                rules[rc].old = strdup(old);
                rules[rc].new_str = strdup(new_str);
                rc++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char out[10000];
            strcpy(out, rest);
            for (int i = 0; i < rc; i++) {
                char *pos = strstr(out, rules[i].old);
                if (pos != NULL) {
                    char temp[10000];
                    int len = strlen(rules[i].old);
                    int prefix = pos - out;
                    strncpy(temp, out, prefix);
                    temp[prefix] = 0;
                    strcat(temp, rules[i].new_str);
                    strcat(temp, pos + len);
                    strcpy(out, temp);
                    break;
                }
            }
            printf("%s\n", out);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_rule(rest);
            if (idx != -1) {
                free(rules[idx].old);
                free(rules[idx].new_str);
                for (int i = idx; i < rc - 1; i++) rules[i] = rules[i+1];
                rc--;
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i, j;
            sscanf(rest, "%d %d", &i, &j);
            if (i >= 0 && i < rc && j >= 0 && j < rc) {
                Rule tmp = rules[i];
                rules[i] = rules[j];
                rules[j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s %s\n", rules[i].old, rules[i].new_str);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(rules[i].old);
        free(rules[i].new_str);
    }
    return 0;
}