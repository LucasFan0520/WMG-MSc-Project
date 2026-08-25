// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *body; } Draft;
Draft *d = NULL;
int n = 0;
Draft *trash = NULL;
int tn = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], title[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "CREATE") == 0) {
            sscanf(line, "%*s %s", title);
            char *body = strchr(line, ' '); body++; body = strchr(body, ' ');
            d = realloc(d, (n + 1) * sizeof(Draft));
            d[n].title = strdup(title); d[n].body = strdup(body ? body + 1 : ""); n++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            sscanf(line, "%*s %s", title);
            char *body = strchr(line, ' '); body++; body = strchr(body, ' ');
            for (int i = 0; i < n; i++) if (strcmp(d[i].title, title) == 0) { free(d[i].body); d[i].body = strdup(body ? body + 1 : ""); break; }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) {
                if (strcmp(d[i].title, title) == 0) {
                    trash = realloc(trash, (tn + 1) * sizeof(Draft));
                    trash[tn].title = strdup(d[i].title); trash[tn].body = strdup(d[i].body); tn++;
                    free(d[i].title); free(d[i].body);
                    for (int j = i; j < n - 1; j++) d[j] = d[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (tn > 0) {
                tn--;
                d = realloc(d, (n + 1) * sizeof(Draft));
                d[n] = trash[tn]; n++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) if (strcmp(d[i].title, title) == 0) { printf("%s %s\n", d[i].title, d[i].body); break; }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < n; i++) printf("%s %s\n", d[i].title, d[i].body);
        }
    }
    for (int i = 0; i < n; i++) { free(d[i].title); free(d[i].body); }
    for (int i = 0; i < tn; i++) { free(trash[i].title); free(trash[i].body); }
    free(d); free(trash);
    return 0;
}