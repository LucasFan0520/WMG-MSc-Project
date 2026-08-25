// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; int rows; int cols; char ***cells; } Table;
Table tables[1000]; int ntables = 0;

int find_table(char *name) {
    for (int i = 0; i < ntables; i++) if (!strcmp(tables[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], val[1000]; int rows, cols, r, c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "TABLE")) {
            scanf("%s %d %d", name, &rows, &cols);
            tables[ntables].name = strdup(name);
            tables[ntables].rows = rows;
            tables[ntables].cols = cols;
            tables[ntables].cells = malloc(sizeof(char**) * rows);
            for (int i = 0; i < rows; i++) {
                tables[ntables].cells[i] = malloc(sizeof(char*) * cols);
                for (int j = 0; j < cols; j++) tables[ntables].cells[i][j] = strdup("EMPTY");
            }
            ntables++;
        } else if (!strcmp(cmd, "SET")) {
            scanf("%s %d %d", name, &r, &c);
            fgets(val, sizeof(val), stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find_table(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                free(tables[idx].cells[r][c]);
                tables[idx].cells[r][c] = strdup(val);
            }
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s %d %d", name, &r, &c);
            int idx = find_table(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                printf("%s\n", tables[idx].cells[r][c]);
            } else {
                printf("EMPTY\n");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_table(name);
            if (idx != -1) {
                free(tables[idx].name);
                for (int i = 0; i < tables[idx].rows; i++) {
                    for (int j = 0; j < tables[idx].cols; j++) free(tables[idx].cells[i][j]);
                    free(tables[idx].cells[i]);
                }
                free(tables[idx].cells);
                for (int i = idx; i < ntables - 1; i++) tables[i] = tables[i + 1];
                ntables--;
            }
        } else if (!strcmp(cmd, "TABLES")) {
            for (int i = 0; i < ntables; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < ntables; i++) {
        free(tables[i].name);
        for (int j = 0; j < tables[i].rows; j++) {
            for (int k = 0; k < tables[i].cols; k++) free(tables[i].cells[j][k]);
            free(tables[i].cells[j]);
        }
        free(tables[i].cells);
    }
    return 0;
}