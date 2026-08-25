// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; int rows, cols; char ***cells; } Table;
Table *tables = NULL;
int tc = 0, tcap = 0;

int find_table(const char *name) {
    for (int i = 0; i < tc; i++)
        if (strcmp(tables[i].name, name) == 0) return i;
    return -1;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name) >= 0) return;
    if (tc == tcap) { tcap = tcap ? tcap * 2 : 4; tables = realloc(tables, sizeof(Table) * tcap); }
    tables[tc].name = strdup(name);
    tables[tc].rows = rows;
    tables[tc].cols = cols;
    tables[tc].cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        tables[tc].cells[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++)
            tables[tc].cells[i][j] = strdup("");
    }
    tc++;
}

void set_cell(const char *name, int row, int col, const char *value) {
    int idx = find_table(name);
    if (idx < 0 || row < 0 || row >= tables[idx].rows || col < 0 || col >= tables[idx].cols) return;
    free(tables[idx].cells[row][col]);
    tables[idx].cells[row][col] = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    int idx = find_table(name);
    if (idx < 0 || row < 0 || row >= tables[idx].rows || col < 0 || col >= tables[idx].cols) {
        printf("EMPTY\n");
        return;
    }
    printf("%s\n", tables[idx].cells[row][col]);
}

void delete_table(const char *name) {
    int idx = find_table(name);
    if (idx < 0) return;
    for (int i = 0; i < tables[idx].rows; i++) {
        for (int j = 0; j < tables[idx].cols; j++)
            free(tables[idx].cells[i][j]);
        free(tables[idx].cells[i]);
    }
    free(tables[idx].cells);
    free(tables[idx].name);
    for (int i = idx; i < tc - 1; i++) tables[i] = tables[i + 1];
    tc--;
}

void list_tables(void) {
    for (int i = 0; i < tc; i++) printf("%s\n", tables[i].name);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], value[4096];
        int rows, cols, row, col;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &rows, &cols) == 3)
                create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %4095[^\n]", name, &row, &col, value) == 4)
                set_cell(name, row, col, value);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &row, &col) == 3)
                get_cell(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) list_tables();
    }
    for (int i = 0; i < tc; i++) {
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++)
                free(tables[i].cells[r][c]);
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
        free(tables[i].name);
    }
    free(tables);
    return 0;
}