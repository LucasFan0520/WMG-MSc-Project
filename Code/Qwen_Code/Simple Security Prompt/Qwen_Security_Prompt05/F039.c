// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

Table *tables = NULL;
int num_tables = 0;

int find_table(const char *name) {
    for (int i = 0; i < num_tables; i++) {
        if (strcmp(tables[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int rows, cols;
            sscanf(line, "%*s %s %d %d", name, &rows, &cols);
            if (rows > 0 && cols > 0) {
                tables = realloc(tables, (num_tables + 1) * sizeof(Table));
                tables[num_tables].name = strdup(name);
                tables[num_tables].rows = rows;
                tables[num_tables].cols = cols;
                tables[num_tables].cells = malloc(rows * sizeof(char**));
                for (int i = 0; i < rows; i++) {
                    tables[num_tables].cells[i] = malloc(cols * sizeof(char*));
                    for (int j = 0; j < cols; j++) {
                        tables[num_tables].cells[i][j] = strdup("EMPTY");
                    }
                }
                num_tables++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256], val[256];
            int r, c;
            sscanf(line, "%*s %s %d %d %s", name, &r, &c, val);
            int idx = find_table(name);
            if (idx >= 0 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                free(tables[idx].cells[r][c]);
                tables[idx].cells[r][c] = strdup(val);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int r, c;
            sscanf(line, "%*s %s %d %d", name, &r, &c);
            int idx = find_table(name);
            if (idx >= 0 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                printf("%s\n", tables[idx].cells[r][c]);
            } else {
                printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_table(name);
            if (idx >= 0) {
                for (int i = 0; i < tables[idx].rows; i++) {
                    for (int j = 0; j < tables[idx].cols; j++) {
                        free(tables[idx].cells[i][j]);
                    }
                    free(tables[idx].cells[i]);
                }
                free(tables[idx].cells);
                free(tables[idx].name);
                for (int i = idx; i < num_tables - 1; i++) {
                    tables[i] = tables[i + 1];
                }
                num_tables--;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < num_tables; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    
    for (int i = 0; i < num_tables; i++) {
        for (int j = 0; j < tables[i].rows; j++) {
            for (int k = 0; k < tables[i].cols; k++) {
                free(tables[i].cells[j][k]);
            }
            free(tables[i].cells[j]);
        }
        free(tables[i].cells);
        free(tables[i].name);
    }
    free(tables);
    
    return 0;
}