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
int tc = 0;

int find_table(const char *name) {
    for (int i = 0; i < tc; i++) {
        if (strcmp(tables[i].name, name) == 0) return i;
    }
    return -1;
}

void free_table(Table *t) {
    free(t->name);
    for (int r = 0; r < t->rows; r++) {
        for (int c = 0; c < t->cols; c++) {
            free(t->cells[r][c]);
        }
        free(t->cells[r]);
    }
    free(t->cells);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char a3[256];
        char a4[1024];
        a1[0] = '\0';
        a2[0] = '\0';
        a3[0] = '\0';
        a4[0] = '\0';
        int n = sscanf(line, "%31s %255s %255s %255s %1023[^\n]", cmd, a1, a2, a3, a4);
        if (n < 1) continue;

        if (strcmp(cmd, "TABLE") == 0 && n >= 4) {
            int rows = atoi(a2);
            int cols = atoi(a3);
            if (rows <= 0 || cols <= 0) continue;
            if (find_table(a1) >= 0) continue;
            tables = realloc(tables, (tc + 1) * sizeof(Table));
            if (!tables) return 1;
            tables[tc].name = strdup(a1);
            tables[tc].rows = rows;
            tables[tc].cols = cols;
            tables[tc].cells = malloc(rows * sizeof(char **));
            if (!tables[tc].cells) return 1;
            for (int r = 0; r < rows; r++) {
                tables[tc].cells[r] = malloc(cols * sizeof(char *));
                if (!tables[tc].cells[r]) return 1;
                for (int c = 0; c < cols; c++) {
                    tables[tc].cells[r][c] = strdup("");
                }
            }
            tc++;
        } else if (strcmp(cmd, "SET") == 0 && n >= 5) {
            int ti = find_table(a1);
            if (ti >= 0) {
                int row = atoi(a2);
                int col = atoi(a3);
                if (row >= 0 && row < tables[ti].rows && col >= 0 && col < tables[ti].cols) {
                    free(tables[ti].cells[row][col]);
                    tables[ti].cells[row][col] = strdup(a4);
                }
            }
        } else if (strcmp(cmd, "GET") == 0 && n >= 4) {
            int ti = find_table(a1);
            if (ti >= 0) {
                int row = atoi(a2);
                int col = atoi(a3);
                if (row >= 0 && row < tables[ti].rows && col >= 0 && col < tables[ti].cols) {
                    if (strlen(tables[ti].cells[row][col]) == 0) {
                        printf("EMPTY\n");
                    } else {
                        printf("%s\n", tables[ti].cells[row][col]);
                    }
                } else {
                    printf("EMPTY\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int ti = find_table(a1);
            if (ti >= 0) {
                free_table(&tables[ti]);
                for (int k = ti; k < tc - 1; k++) {
                    tables[k] = tables[k + 1];
                }
                tc--;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < tc; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < tc; i++) {
        free_table(&tables[i]);
    }
    free(tables);
    return 0;
}