// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } Var;
Var *vars = 0; int n = 0, cap = 0;

char* get_val(char *key) {
    for (int i=0; i<n; i++) if (!strcmp(vars[i].key, key)) return vars[i].val;
    return 0;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[1700];
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "SET") || !strcmp(cmd, "RENDER")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "SET")) {
            int i = find(a); // Assuming find is defined or inline it
            int idx = -1; for(int i=0;i<n;i++) if(!strcmp(vars[i].key, a)) {idx=i; break;}
            if (idx >= 0) { free(vars[idx].val); vars[idx].val = strdup(rest); }
            else {
                if (n == cap) { cap = cap ? cap*2 : 4; vars = realloc(vars, cap*sizeof(Var)); }
                vars[n].key = strdup(a); vars[n].val = strdup(rest); n++;
            }
        } else if (!strcmp(cmd, "UNSET")) {
            int idx = -1; for(int i=0;i<n;i++) if(!strcmp(vars[i].key, a)) {idx=i; break;}
            if (idx >= 0) {
                free(vars[idx].key); free(vars[idx].val);
                for (int j=idx; j<n-1; j++) vars[j] = vars[j+1]; n--;
            }
        } else if (!strcmp(cmd, "RENDER")) {
            char out[4096] = {0}; int oi = 0;
            for (char *p = rest; *p; ) {
                if (*p == '{') {
                    char *end = strchr(p, '}');
                    if (end) {
                        char key[256]; strncpy(key, p+1, end-p-1); key[end-p-1] = 0;
                        char *v = get_val(key);
                        if (v) { strcpy(out+oi, v); oi += strlen(v); p = end+1; continue; }
                    }
                }
                out[oi++] = *p++;
            }
            puts(out);
        } else if (!strcmp(cmd, "VARS")) {
            for (int i=0; i<n; i++) printf("%s %s\n", vars[i].key, vars[i].val);
        }
    }
    return 0;
}