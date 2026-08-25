// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Var;
Var *v = NULL;
int n = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], k[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s", k);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' ');
            int idx = -1;
            for (int i = 0; i < n; i++) if (strcmp(v[i].key, k) == 0) { idx = i; break; }
            if (idx != -1) { free(v[idx].val); v[idx].val = strdup(t ? t + 1 : ""); }
            else { v = realloc(v, (n + 1) * sizeof(Var)); v[n].key = strdup(k); v[n].val = strdup(t ? t + 1 : ""); n++; }
        } else if (strcmp(cmd, "UNSET") == 0) {
            sscanf(line, "%*s %s", k);
            for (int i = 0; i < n; i++) {
                if (strcmp(v[i].key, k) == 0) {
                    free(v[i].key); free(v[i].val);
                    for (int j = i; j < n - 1; j++) v[j] = v[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *t = strchr(line, ' ');
            if (!t) { printf("\n"); continue; }
            t++;
            char out[4096] = "";
            int oi = 0;
            while (*t) {
                if (*t == '{') {
                    char *end = strchr(t, '}');
                    if (end) {
                        char tk[256];
                        strncpy(tk, t + 1, end - t - 1);
                        tk[end - t - 1] = 0;
                        int found = 0;
                        for (int i = 0; i < n; i++) {
                            if (strcmp(v[i].key, tk) == 0) {
                                strcpy(out + oi, v[i].val);
                                oi += strlen(v[i].val);
                                found = 1; break;
                            }
                        }
                        if (!found) {
                            strncpy(out + oi, t, end - t + 1);
                            oi += end - t + 1;
                        }
                        t = end + 1;
                    } else { out[oi++] = *t++; }
                } else { out[oi++] = *t++; }
            }
            out[oi] = 0;
            printf("%s\n", out);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (int i = 0; i < n; i++) printf("%s=%s\n", v[i].key, v[i].val);
        }
    }
    for (int i = 0; i < n; i++) { free(v[i].key); free(v[i].val); }
    free(v);
    return 0;
}