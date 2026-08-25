// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} CountedTable;

int main() {
    CountedTable *tables = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = strtok(NULL, " ");
            char *r_str = strtok(NULL, " ");
            char *c_str = strtok(NULL, " ");
            if (name && r_str && c_str) {
                int rows = atoi(r_str);
                int cols = atoi(c_str);
                if (rows > 0 && cols > 0) {
                    tables = realloc(tables, (count + 1) * sizeof(CountedTable));
                    tables[count].name = strdup(name);
                    tables[count].rows = rows;
                    tables[count].cols = cols;
                    tables[count].cells = malloc(rows * sizeof(char **));
                    for (int i = 0; i < rows; i++) {
                        tables[count].cells[i] = calloc(cols, sizeof(char *));
                    }
                    count++;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = strtok(NULL, " ");
            char *r_str = strtok(NULL, " ");
            char *c_str = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            if (name && r_str && c_str && val) {
                int r = atoi(r_str);
                int c = atoi(c_str);
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
                            tables[i].cells[r][c] = strdup(val);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = strtok(NULL, " ");
            char *r_str = strtok(NULL, " ");
            char *c_str = strtok(NULL, " ");
            if (name && r_str && c_str) {
                int r = atoi(r_str);
                int c = atoi(c_str);
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        found = 1;
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols && tables[i].cells[r][c]) {
                            printf("%s\n", tables[i].cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                        break;
                    }
                }
                if (!found) printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        free(tables[i].name);
                        for (int r = 0; r < tables[i].rows; r++) {
                            for (int c = 0; c < tables[i].cols; c++) {
                                if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
                            }
                            free(tables[i].cells[r]);
                        }
                        free(tables[i].cells);
                        for (int j = i; j < count - 1; j++) {
                            tables[j] = tables[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(tables);
                    tables = NULL;
                } else {
                    tables = realloc(tables, count * sizeof(CountedTable));
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(tables[i].name);
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++) {
                if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
