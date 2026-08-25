// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; int rows; int cols; char ***cells; } Tab;
Tab *tabs = NULL;
int nt = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        int r, c;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "TABLE") == 0) {
            sscanf(line, "%*s %s %d %d", a, &r, &c);
            if (r < 0 || c < 0) continue;
            tabs = realloc(tabs, (nt + 1) * sizeof(Tab));
            tabs[nt].name = strdup(a);
            tabs[nt].rows = r;
            tabs[nt].cols = c;
            tabs[nt].cells = malloc(r * sizeof(char**));
            for (int i = 0; i < r; i++) {
                tabs[nt].cells[i] = malloc(c * sizeof(char*));
                for (int j = 0; j < c; j++) tabs[nt].cells[i][j] = NULL;
            }
            nt++;
        } else if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s %d %d", a, &r, &c);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++; t = strchr(t, ' '); t++; t = strchr(t, ' '); t++;
            for (int i = 0; i < nt; i++) {
                if (strcmp(tabs[i].name, a) == 0) {
                    if (r >= 0 && r < tabs[i].rows && c >= 0 && c < tabs[i].cols) {
                        free(tabs[i].cells[r][c]);
                        tabs[i].cells[r][c] = strdup(t);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            sscanf(line, "%*s %s %d %d", a, &r, &c);
            for (int i = 0; i < nt; i++) {
                if (strcmp(tabs[i].name, a) == 0) {
                    if (r >= 0 && r < tabs[i].rows && c >= 0 && c < tabs[i].cols) {
                        printf("%s\n", tabs[i].cells[r][c] ? tabs[i].cells[r][c] : "EMPTY");
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nt; i++) {
                if (strcmp(tabs[i].name, a) == 0) {
                    for (int r = 0; r < tabs[i].rows; r++) {
                        for (int c = 0; c < tabs[i].cols; c++) free(tabs[i].cells[r][c]);
                        free(tabs[i].cells[r]);
                    }
                    free(tabs[i].cells);
                    free(tabs[i].name);
                    for (int j = i; j < nt - 1; j++) tabs[j] = tabs[j + 1];
                    nt--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < nt; i++) {
                printf("%s\n", tabs[i].name);
            }
        }
    }
    for (int i = 0; i < nt; i++) {
        for (int r = 0; r < tabs[i].rows; r++) {
            for (int c = 0; c < tabs[i].cols; c++) free(tabs[i].cells[r][c]);
            free(tabs[i].cells[r]);
        }
        free(tabs[i].cells);
        free(tabs[i].name);
    }
    free(tabs);
    return 0;
}