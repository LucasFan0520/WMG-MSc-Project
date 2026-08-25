// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells;
    struct Table *next;
} Table;

Table *tables = NULL;

Table *find_table(const char *name) {
    Table *curr = tables;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows <= 0 || cols <= 0) return;
    
    Table *t = malloc(sizeof(Table));
    if (!t) exit(EXIT_FAILURE);
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    
    t->cells = malloc(rows * sizeof(char **));
    if (!t->cells) exit(EXIT_FAILURE);
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        if (!t->cells[i]) exit(EXIT_FAILURE);
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = NULL;
        }
    }
    
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    
    free(t->cells[row][col]);
    t->cells[row][col] = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    
    if (t->cells[row][col]) {
        printf("%s\n", t->cells[row][col]);
    } else {
        printf("EMPTY\n");
    }
}

void delete_table(const char *name) {
    Table *curr = tables;
    Table *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else tables = curr->next;
            
            for (int i = 0; i < curr->rows; i++) {
                for (int j = 0; j < curr->cols; j++) {
                    free(curr->cells[i][j]);
                }
                free(curr->cells[i]);
            }
            free(curr->cells);
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_tables() {
    Table *curr = tables;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
    }
}

void free_all() {
    Table *curr = tables;
    while (curr) {
        Table *next = curr->next;
        for (int i = 0; i < curr->rows; i++) {
            for (int j = 0; j < curr->cols; j++) {
                free(curr->cells[i][j]);
            }
            free(curr->cells[i]);
        }
        free(curr->cells);
        free(curr->name);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char arg4[3200] = {0};
        int r = 0, c = 0;
        
        int matched = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        if (matched >= 2) sscanf(arg2, "%d", &r);
        if (matched >= 3) sscanf(arg3, "%d", &c);
        
        if (strcmp(cmd, "TABLE") == 0 && matched >= 3) {
            int cols = 0;
            sscanf(arg3, "%d", &cols);
            create_table(arg1, r, cols);
        } else if (strcmp(cmd, "SET") == 0 && matched >= 4) {
            set_cell(arg1, r, c, arg4);
        } else if (strcmp(cmd, "GET") == 0 && matched >= 3) {
            get_cell(arg1, r, c);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_table(arg1);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}