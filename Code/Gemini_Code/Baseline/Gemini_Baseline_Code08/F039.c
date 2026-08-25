// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} CountedTable;

CountedTable *tables = NULL;
int count = 0;
int capacity = 0;

int find_table(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name) != -1) return;
    if (rows <= 0 || cols <= 0) return;
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        tables = realloc(tables, capacity * sizeof(CountedTable));
    }
    tables[count].name = strdup(name);
    tables[count].rows = rows;
    tables[count].cols = cols;
    tables[count].cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        tables[count].cells[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            tables[count].cells[i][j] = NULL;
        }
    }
    count++;
}

void set_cell(const char *name, int r, int c, const char *value) {
    int idx = find_table(name);
    if (idx == -1) return;
    CountedTable *t = &tables[idx];
    if (r < 0 || r >= t->rows || c < 0 || c >= t->cols) return;
    if (t->cells[r][c]) free(t->cells[r][c]);
    t->cells[r][c] = strdup(value);
}

void get_cell(const char *name, int r, int c) {
    int idx = find_table(name);
    if (idx == -1) {
        printf("EMPTY\n");
        return;
    }
    CountedTable *t = &tables[idx];
    if (r < 0 || r >= t->rows || c < 0 || c >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    if (t->cells[r][c]) {
        printf("%s\n", t->cells[r][c]);
    } else {
        printf("EMPTY\n");
    }
}

void delete_table(const char *name) {
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
        for (int i = idx; i < count - 1; i++) {
            tables[i] = tables[i + 1];
        }
        count--;
    }
}

void list_tables(void) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", tables[i].name);
    }
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = strtok(NULL, " ");
            char *r_s = strtok(NULL, " ");
            char *c_s = strtok(NULL, "");
            if (name && r_s && c_s) {
                create_table(name, atoi(r_s), atoi(c_s));
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = strtok(NULL, " ");
            char *r_s = strtok(NULL, " ");
            char *c_s = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (name && r_s && c_s && val) {
                set_cell(name, atoi(r_s), atoi(c_s), val);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = strtok(NULL, " ");
            char *r_s = strtok(NULL, " ");
            char *c_s = strtok(NULL, "");
            if (name && r_s && c_s) {
                get_cell(name, atoi(r_s), atoi(c_s));
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free(line);
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
