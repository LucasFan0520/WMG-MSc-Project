// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Var;

Var vars[1000];
int vc = 0;

int find_key(char *key) {
    for (int i = 0; i < vc; i++) if (strcmp(vars[i].key, key) == 0) return i;
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
        
        if (strcmp(cmd, "SET") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            char *val = rest + strlen(key);
            while (*val == ' ') val++;
            int idx = find_key(key);
            if (idx == -1) {
                vars[vc].key = strdup(key);
                vars[vc].val = strdup(val);
                vc++;
            } else {
                free(vars[idx].val);
                vars[idx].val = strdup(val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(vars[idx].key);
                free(vars[idx].val);
                for (int i = idx; i < vc - 1; i++) vars[i] = vars[i+1];
                vc--;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char out[10000] = "";
            char *p = rest;
            while (*p) {
                if (*p == '{') {
                    char key[1000] = "";
                    int k = 0;
                    p++;
                    while (*p && *p != '}' && k < 999) key[k++] = *p++;
                    key[k] = 0;
                    if (*p == '}') p++;
                    int idx = find_key(key);
                    if (idx != -1) strcat(out, vars[idx].val);
                    else {
                        strcat(out, "{");
                        strcat(out, key);
                        strcat(out, "}");
                    }
                } else {
                    int k = strlen(out);
                    out[k++] = *p++;
                    out[k] = 0;
                }
            }
            printf("%s\n", out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < vc; i++) printf("%s=%s\n", vars[i].key, vars[i].val);
        }
    }
    for (int i = 0; i < vc; i++) {
        free(vars[i].key);
        free(vars[i].val);
    }
    return 0;
}