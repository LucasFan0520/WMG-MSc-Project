// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *old, *new; } Rule;
Rule *rules; int n_rule;

int find_rule(char *old) {
    for(int i=0; i<n_rule; i++) if(!strcmp(rules[i].old, old)) return i;
    return -1;
}

int main() {
    char cmd[20], old[100], nw[100], text[1000]; int i, j;
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ADD")) {
            scanf("%s %s", old, nw);
            rules = realloc(rules, (n_rule+1)*sizeof(Rule));
            rules[n_rule].old = strdup(old);
            rules[n_rule].new = strdup(nw);
            n_rule++;
        } else if(!strcmp(cmd, "APPLY")) {
            scanf("%s", text);
            int found = 0;
            for(int k=0; k<n_rule; k++) {
                if(strstr(text, rules[k].old)) {
                    char res[2000] = "";
                    char *p = text;
                    int len = strlen(rules[k].old);
                    while(*p) {
                        if(strncmp(p, rules[k].old, len) == 0) {
                            strcat(res, rules[k].new);
                            p += len;
                        } else {
                            char tmp[2] = {*p, 0};
                            strcat(res, tmp);
                            p++;
                        }
                    }
                    printf("%s\n", res);
                    found = 1;
                    break;
                }
            }
            if(!found) printf("%s\n", text);
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", old);
            int idx = find_rule(old);
            if(idx != -1) {
                free(rules[idx].old);
                free(rules[idx].new);
                for(int k=idx; k<n_rule-1; k++) rules[k] = rules[k+1];
                n_rule--;
            }
        } else if(!strcmp(cmd, "SWAP")) {
            scanf("%d %d", &i, &j);
            if(i >= 0 && i < n_rule && j >= 0 && j < n_rule) {
                Rule tmp = rules[i];
                rules[i] = rules[j];
                rules[j] = tmp;
            }
        } else if(!strcmp(cmd, "RULES")) {
            for(int k=0; k<n_rule; k++) {
                printf("%s %s\n", rules[k].old, rules[k].new);
            }
        }
    }
    return 0;
}