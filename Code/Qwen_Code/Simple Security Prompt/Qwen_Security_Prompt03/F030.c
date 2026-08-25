// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *old_str; char *new_str; } Rule;
Rule *rules = NULL;
int nr = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256];
        int i, j;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            rules = realloc(rules, (nr + 1) * sizeof(Rule));
            rules[nr].old_str = strdup(a);
            rules[nr].new_str = strdup(b);
            nr++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            int applied = 0;
            for (int k = 0; k < nr; k++) {
                char *pos = strstr(t, rules[k].old_str);
                if (pos) {
                    char res[4096];
                    strncpy(res, t, pos - t);
                    res[pos - t] = 0;
                    strcat(res, rules[k].new_str);
                    strcat(res, pos + strlen(rules[k].old_str));
                    printf("%s\n", res);
                    applied = 1;
                    break;
                }
            }
            if (!applied) printf("%s\n", t);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int k = 0; k < nr; k++) {
                if (strcmp(rules[k].old_str, a) == 0) {
                    free(rules[k].old_str);
                    free(rules[k].new_str);
                    for (int m = k; m < nr - 1; m++) rules[m] = rules[m + 1];
                    nr--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            sscanf(line, "%*s %d %d", &i, &j);
            if (i >= 0 && i < nr && j >= 0 && j < nr) {
                Rule tmp = rules[i];
                rules[i] = rules[j];
                rules[j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int k = 0; k < nr; k++) {
                printf("%s %s\n", rules[k].old_str, rules[k].new_str);
            }
        }
    }
    for (int k = 0; k < nr; k++) {
        free(rules[k].old_str);
        free(rules[k].new_str);
    }
    free(rules);
    return 0;
}