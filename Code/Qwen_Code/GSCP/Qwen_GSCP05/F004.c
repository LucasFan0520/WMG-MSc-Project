// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Var;
Var *vars = NULL; int vc = 0;

int find_var(const char *k) {
    for (int i = 0; i < vc; i++) if (strcmp(vars[i].key, k) == 0) return i;
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
        
        if (strcmp(c1, "SET") == 0) {
            int i = find_var(c2);
            if (i >= 0) { free(vars[i].val); vars[i].val = strdup(c3); }
            else { vars = realloc(vars, (vc+1)*sizeof(Var)); vars[vc].key = strdup(c2); vars[vc].val = strdup(c3); vc++; }
        } else if (strcmp(c1, "UNSET") == 0) {
            int i = find_var(c2);
            if (i >= 0) { free(vars[i].key); free(vars[i].val); for(int j=i; j<vc-1; j++) vars[j]=vars[j+1]; vc--; }
        } else if (strcmp(c1, "RENDER") == 0) {
            char out[2048] = "";
            char *p = c2;
            while (*p) {
                if (*p == '{') {
                    char *end = strchr(p, '}');
                    if (end) {
                        *end = 0;
                        int i = find_var(p + 1);
                        if (i >= 0) strcat(out, vars[i].val);
                        else { strncat(out, p, end - p + 1); }
                        p = end + 1;
                    } else {
                        strcat(out, "{");
                        p++;
                    }
                } else {
                    char t[2] = {*p, 0};
                    strcat(out, t);
                    p++;
                }
            }
            printf("%s\n", out);
        } else if (strcmp(c1, "VARS") == 0) {
            for (int i = 0; i < vc; i++) printf("%s %s\n", vars[i].key, vars[i].val);
        }
    }
    for (int i = 0; i < vc; i++) { free(vars[i].key); free(vars[i].val); }
    free(vars);
    return 0;
}