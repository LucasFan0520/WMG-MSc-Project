/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} CountedTable;

int main(void) {
    CountedTable *tables = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *args = line + 6;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *name = args;
            char *rest = p1 + 1;
            char *p2 = strchr(rest, ' ');
            if (!p2) { *p1 = ' '; continue; }
            *p2 = '\0';
            int r = atoi(rest);
            int c = atoi(p2 + 1);
            *p1 = ' '; *p2 = ' ';
            if (r <= 0 || c <= 0 || r > 1000 || c > 1000) continue;
            char *n_name = strdup(name);
            char ***t_cells = calloc(r, sizeof(char **));
            if (n_name && t_cells) {
                int ok = 1;
                for (int i = 0; i < r; i++) {
                    t_cells[i] = calloc(c, sizeof(char *));
                    if (!t_cells[i]) ok = 0;
                }
                if (ok) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        CountedTable *tmp = realloc(tables, new_cap * sizeof(CountedTable));
                        if (tmp) {
                            tables = tmp;
                            capacity = new_cap;
                        } else {
                            ok = 0;
                        }
                    }
                    if (ok) {
                        tables[count].name = n_name;
                        tables[count].rows = r;
                        tables[count].cols = c;
                        tables[count].cells = t_cells;
                        count++;
                    }
                }
                if (!ok) {
                    free(n_name);
                    if (t_cells) {
                        for (int i = 0; i < r; i++) free(t_cells[i]);
                        free(t_cells);
                    }
                }
            } else {
                free(n_name); free(t_cells);
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *args = line + 4;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *name = args;
            char *rest1 = p1 + 1;
            char *p2 = strchr(rest1, ' ');
            if (!p2) { *p1 = ' '; continue; }
            *p2 = '\0';
            int row_idx = atoi(rest1);
            char *rest2 = p2 + 1;
            char *p3 = strchr(rest2, ' ');
            char *val = NULL;
            if (p3) {
                *p3 = '\0';
                val = p3 + 1;
            } else {
                val = "";
            }
            int col_idx = atoi(rest2);
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (row_idx >= 0 && row_idx < tables[i].rows && col_idx >= 0 && col_idx < tables[i].cols) {
                        char *n_val = strdup(val);
                        if (n_val) {
                            free(tables[i].cells[row_idx][col_idx]);
                            tables[i].cells[row_idx][col_idx] = n_val;
                        }
                    }
                    break;
                }
            }
            *p1 = ' '; *p2 = ' '; if (p3) *p3 = ' ';
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *args = line + 4;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *name = args;
            char *rest = p1 + 1;
            char *p2 = strchr(rest, ' ');
            if (!p2) { *p1 = ' '; continue; }
            *p2 = '\0';
            int row_idx = atoi(rest);
            int col_idx = atoi(p2 + 1);
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (row_idx >= 0 && row_idx < tables[i].rows && col_idx >= 0 && col_idx < tables[i].cols) {
                        if (tables[i].cells[row_idx][col_idx]) {
                            printf("%s\n", tables[i].cells[row_idx][col_idx]);
                        } else {
                            printf("EMPTY\n");
                        }
                    }
                    break;
                }
            }
            *p1 = ' '; *p2 = ' ';
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
                    break;
                }
            }
        } else if (strcmp(line, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    free(line);
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
