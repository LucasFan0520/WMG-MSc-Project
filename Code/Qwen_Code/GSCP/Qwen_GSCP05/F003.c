// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *exp; } Alias;
Alias *als = NULL; int ac = 0;

int find_alias(const char *n) {
    for (int i = 0; i < ac; i++) if (strcmp(als[i].name, n) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[700];
        c2[0] = c3[0] = 0;
        sscanf(line, "%19s %255s %700[^\n]", c1, c2, c3);
        
        if (strcmp(c1, "DEFINE") == 0) {
            int i = find_alias(c2);
            if (i >= 0) { free(als[i].exp); als[i].exp = strdup(c3); }
            else { als = realloc(als, (ac+1)*sizeof(Alias)); als[ac].name = strdup(c2); als[ac].exp = strdup(c3); ac++; }
        } else if (strcmp(c1, "EXPAND") == 0) {
            int i = find_alias(c2);
            if (i >= 0) { for(char *p=als[i].exp; *p; p++) putchar(*p==' '?'_':*p); putchar('\n'); }
        } else if (strcmp(c1, "RENAME") == 0) {
            int i = find_alias(c2);
            if (i >= 0) { free(als[i].name); als[i].name = strdup(c3); }
        } else if (strcmp(c1, "ERASE") == 0) {
            int i = find_alias(c2);
            if (i >= 0) { free(als[i].name); free(als[i].exp); for(int j=i; j<ac-1; j++) als[j]=als[j+1]; ac--; }
        } else if (strcmp(c1, "DUMP") == 0) {
            for (int i = 0; i < ac; i++) printf("%s %s\n", als[i].name, als[i].exp);
        }
    }
    for (int i = 0; i < ac; i++) { free(als[i].name); free(als[i].exp); }
    free(als);
    return 0;
}