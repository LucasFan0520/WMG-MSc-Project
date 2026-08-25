// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} DataTable;

int main() {
    DataTable *tables = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int r = 0, c = 0;
            if (sscanf(args, "%255s %d %d", name, &r, &c) == 3) {
                if (r > 0 && c > 0) {
                    tables = realloc(tables, (count + 1) * sizeof(DataTable));
                    tables[count].name = strdup(name);
                    tables[count].rows = r;
                    tables[count].cols = c;
                    tables[count].cells = malloc(r * sizeof(char **));
                    for (int i = 0; i < r; i++) {
                        tables[count].cells[i] = malloc(c * sizeof(char *));
                        for (int j = 0; j < c; j++) {
                            tables[count].cells[i][j] = strdup("EMPTY");
                        }
                    }
                    count++;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256], val[256];
            int r = 0, c = 0;
            if (sscanf(args, "%255s %d %d %255s", name, &r, &c, val) == 4) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            free(tables[i].cells[r][c]);
                            tables[i].cells[r][c] = strdup(val);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int r = 0, c = 0;
            if (sscanf(args, "%255s %d %d", name, &r, &c) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            printf("%s\n", tables[i].cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        free(tables[i].name);
                        for (int r = 0; r < tables[i].rows; r++) {
                            for (int c = 0; c < tables[i].cols; c++) {
                                free(tables[i].cells[r][c]);
                            }
                            free(tables[i].cells[r]);
                        }
                        free(tables[i].cells);
                        for (int j = i; j < count - 1; j++) {
                            tables[j] = tables[j + 1];
                        }
                        count--;
                        tables = realloc(tables, count * sizeof(DataTable));
                        break;
                    }
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
                free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
