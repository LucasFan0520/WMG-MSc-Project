// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_VAL 100

typedef struct {
    char name[MAX_NAME];
    int rows;
    int cols;
    char ***data;
} Table;

Table tables[100];
int tab_count = 0;

int find_tab(const char *name) {
    for (int i = 0; i < tab_count; i++) if (strcmp(tables[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], value[MAX_VAL];
    int rows, cols, r, c;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "TABLE") == 0) {
            scanf("%s %d %d", name, &rows, &cols);
            int idx = tab_count++;
            strcpy(tables[idx].name, name);
            tables[idx].rows = rows;
            tables[idx].cols = cols;
            tables[idx].data = (char ***)malloc(rows * sizeof(char **));
            for (int i = 0; i < rows; i++) {
                tables[idx].data[i] = (char **)malloc(cols * sizeof(char *));
                for (int j = 0; j < cols; j++) {
                    tables[idx].data[i][j] = NULL;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            scanf("%s %d %d %s", name, &r, &c, value);
            int idx = find_tab(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                if (tables[idx].data[r][c]) free(tables[idx].data[r][c]);
                tables[idx].data[r][c] = strdup(value);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s %d %d", name, &r, &c);
            int idx = find_tab(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                if (tables[idx].data[r][c]) printf("%s\n", tables[idx].data[r][c]);
                else printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_tab(name);
            if (idx != -1) {
                for (int i = 0; i < tables[idx].rows; i++) {
                    for (int j = 0; j < tables[idx].cols; j++) {
                        if (tables[idx].data[i][j]) free(tables[idx].data[i][j]);
                    }
                    free(tables[idx].data[i]);
                }
                free(tables[idx].data);
                for (int i = idx; i < tab_count - 1; i++) tables[i] = tables[i + 1];
                tab_count--;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < tab_count; i++) printf("%s\n", tables[i].name);
        }
    }
    return 0;
}