// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

int main(void) {
    Table *tables = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *r_str = space1 + 1;
            while (*r_str == ' ') r_str++;
            char *space2 = strchr(r_str, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *c_str = space2 + 1;
            while (*c_str == ' ') c_str++;
            int rows = atoi(r_str);
            int cols = atoi(c_str);
            if (rows <= 0 || cols <= 0 || rows > 1000 || cols > 1000) continue;
            
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Table *tmp = realloc(tables, new_cap * sizeof(Table));
                if (!tmp) return 1;
                tables = tmp;
                capacity = new_cap;
            }
            tables[count].name = strdup(name);
            tables[count].rows = rows;
            tables[count].cols = cols;
            char ***cells = malloc(rows * sizeof(char **));
            if (!cells) return 1;
            for (int r = 0; r < rows; r++) {
                cells[r] = malloc(cols * sizeof(char *));
                if (!cells[r]) return 1;
                for (int c = 0; c < cols; c++) {
                    cells[r][c] = strdup("EMPTY");
                }
            }
            tables[count].cells = cells;
            count++;
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *r_str = space1 + 1;
            while (*r_str == ' ') r_str++;
            char *space2 = strchr(r_str, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *c_str = space2 + 1;
            while (*c_str == ' ') c_str++;
            char *space3 = strchr(c_str, ' ');
            char *val = "";
            if (space3) {
                *space3 = '\0';
                val = space3 + 1;
                while (*val == ' ') val++;
            }
            int row = atoi(r_str);
            int col = atoi(c_str);
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                Table *t = &tables[found];
                if (row >= 0 && row < t->rows && col >= 0 && col < t->cols) {
                    free(t->cells[row][col]);
                    t->cells[row][col] = strdup(val);
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *r_str = space1 + 1;
            while (*r_str == ' ') r_str++;
            char *space2 = strchr(r_str, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *c_str = space2 + 1;
            while (*c_str == ' ') c_str++;
            int row = atoi(r_str);
            int col = atoi(c_str);
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                Table *t = &tables[found];
                if (row >= 0 && row < t->rows && col >= 0 && col < t->cols) {
                    printf("%s\n", t->cells[row][col]);
                } else {
                    printf("INVALID\n");
                }
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(tables[found].name);
                for (int r = 0; r < tables[found].rows; r++) {
                    for (int c = 0; c < tables[found].cols; c++) {
                        free(tables[found].cells[r][c]);
                    }
                    free(tables[found].cells[r]);
                }
                free(tables[found].cells);
                for (int i = found; i < count - 1; i++) {
                    tables[i] = tables[i + 1];
                }
                count--;
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
