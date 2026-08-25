// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *exp; } Alias;
Alias *a = NULL;
int n = 0;

void print_under(const char *s) { for(;*s;s++) printf("%c", *s==' '?'_':*s); printf("\n"); }

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], x[256], y[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "DEFINE") == 0) {
            sscanf(line, "%*s %s", x);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' ');
            int idx = -1;
            for (int i = 0; i < n; i++) if (strcmp(a[i].name, x) == 0) { idx = i; break; }
            if (idx != -1) { free(a[idx].exp); a[idx].exp = strdup(t ? t + 1 : ""); }
            else { a = realloc(a, (n + 1) * sizeof(Alias)); a[n].name = strdup(x); a[n].exp = strdup(t ? t + 1 : ""); n++; }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            sscanf(line, "%*s %s", x);
            for (int i = 0; i < n; i++) if (strcmp(a[i].name, x) == 0) { print_under(a[i].exp); break; }
        } else if (strcmp(cmd, "RENAME") == 0) {
            sscanf(line, "%*s %s %s", x, y);
            for (int i = 0; i < n; i++) if (strcmp(a[i].name, x) == 0) { free(a[i].name); a[i].name = strdup(y); break; }
        } else if (strcmp(cmd, "ERASE") == 0) {
            sscanf(line, "%*s %s", x);
            for (int i = 0; i < n; i++) {
                if (strcmp(a[i].name, x) == 0) {
                    free(a[i].name); free(a[i].exp);
                    for (int j = i; j < n - 1; j++) a[j] = a[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < n; i++) printf("%s %s\n", a[i].name, a[i].exp);
        }
    }
    for (int i = 0; i < n; i++) { free(a[i].name); free(a[i].exp); }
    free(a);
    return 0;
}