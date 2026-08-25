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

Table *head = NULL;

Table *find_table(const char *name) {
    for (Table *t = head; t; t = t->next) {
        if (strcmp(t->name, name) == 0) return t;
    }
    return NULL;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows <= 0 || cols <= 0) return;
    
    Table *t = malloc(sizeof(Table));
    if (!t) return;
    t->name = strdup(name);
    if (!t->name) { free(t); return; }
    t->rows = rows;
    t->cols = cols;
    
    t->cells = malloc(rows * sizeof(char**));
    if (!t->cells) { free(t->name); free(t); return; }
    
    for (int i = 0; i < rows; i++) {
        t->cells[i] = calloc(cols, sizeof(char*));
        if (!t->cells[i]) {
            for (int j = 0; j < i; j++) free(t->cells[j]);
            free(t->cells);
            free(t->name);
            free(t);
            return;
        }
    }
    
    t->next = head;
    head = t;
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
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    if (t->cells[row][col]) {
        printf("%s\n", t->cells[row][col]);
    } else {
        printf("EMPTY\n");
    }
}

void delete_table(const char *name) {
    Table *curr = head;
    Table *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            
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
    for (Table *t = head; t; t = t->next) {
        printf("%s\n", t->name);
    }
}

void free_all() {
    Table *t = head;
    while (t) {
        Table *next = t->next;
        for (int i = 0; i < t->rows; i++) {
            for (int j = 0; j < t->cols; j++) {
                free(t->cells[i][j]);
            }
            free(t->cells[i]);
        }
        free(t->cells);
        free(t->name);
        free(t);
        t = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        char arg3[1024] = {0};
        char arg4[4096] = {0};
        int r = 0, c = 0;
        
        int n = sscanf(line, "%63s %1023s %1023s %1023s %4095[^\n]", cmd, arg1, arg2, arg3, arg4);
        if (n < 1) continue;
        
        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %1023s %d %d", arg1, &r, &c) == 3) {
                create_table(arg1, r, c);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %1023s %d %d %4095[^\n]", arg1, &r, &c, arg4) == 4) {
                set_cell(arg1, r, c, arg4);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %1023s %d %d", arg1, &r, &c) == 3) {
                get_cell(arg1, r, c);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_table(arg1);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}