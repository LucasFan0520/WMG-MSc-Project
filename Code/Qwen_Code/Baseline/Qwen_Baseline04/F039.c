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
int tcount = 0;

int find_table(const char *name) {
    for (int i = 0; i < tcount; i++) {
        if (strcmp(tables[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[100];
            int rows, cols;
            scanf("%s %d %d", name, &rows, &cols);
            if (find_table(name) == -1) {
                tables = realloc(tables, sizeof(Table) * (tcount + 1));
                tables[tcount].name = strdup(name);
                tables[tcount].rows = rows;
                tables[tcount].cols = cols;
                tables[tcount].cells = malloc(sizeof(char**) * rows);
                for (int i = 0; i < rows; i++) {
                    tables[tcount].cells[i] = malloc(sizeof(char*) * cols);
                    for (int j = 0; j < cols; j++) {
                        tables[tcount].cells[i][j] = NULL;
                    }
                }
                tcount++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[100], val[1000];
            int r, c;
            scanf("%s %d %d", name, &r, &c);
            scanf(" %[^\n]", val);
            int idx = find_table(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                if (tables[idx].cells[r][c]) free(tables[idx].cells[r][c]);
                tables[idx].cells[r][c] = strdup(val);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[100];
            int r, c;
            scanf("%s %d %d", name, &r, &c);
            int idx = find_table(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                if (tables[idx].cells[r][c]) {
                    printf("%s\n", tables[idx].cells[r][c]);
                } else {
                    printf("EMPTY\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_table(name);
            if (idx != -1) {
                free(tables[idx].name);
                for (int i = 0; i < tables[idx].rows; i++) {
                    for (int j = 0; j < tables[idx].cols; j++) {
                        if (tables[idx].cells[i][j]) free(tables[idx].cells[i][j]);
                    }
                    free(tables[idx].cells[i]);
                }
                free(tables[idx].cells);
                for (int i = idx; i < tcount - 1; i++) tables[i] = tables[i+1];
                tcount--;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < tcount; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < tcount; i++) {
        free(tables[i].name);
        for (int j = 0; j < tables[i].rows; j++) {
            for (int k = 0; k < tables[i].cols; k++) {
                if (tables[i].cells[j][k]) free(tables[i].cells[j][k]);
            }
            free(tables[i].cells[j]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}