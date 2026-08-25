// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cell {
    char *value;
    struct cell *next;
};

struct row {
    struct cell *cells;
    struct row *next;
};

struct table {
    char *name;
    int rows;
    int cols;
    struct row *row_data;
    struct table *next;
};

struct table *tables = NULL;

struct table *find_table(char *name) {
    struct table *t = tables;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void create_table(char *name, int rows, int cols) {
    if (find_table(name)) return;
    struct table *t = malloc(sizeof(struct table));
    t->name = malloc(strlen(name) + 1);
    strcpy(t->name, name);
    t->rows = rows;
    t->cols = cols;
    t->row_data = NULL;
    // initialize rows with empty cells
    struct row *prev = NULL;
    for (int i = 0; i < rows; i++) {
        struct row *r = malloc(sizeof(struct row));
        r->cells = NULL;
        struct cell *prev_cell = NULL;
        for (int j = 0; j < cols; j++) {
            struct cell *c = malloc(sizeof(struct cell));
            c->value = malloc(1);
            c->value[0] = 0;
            c->next = NULL;
            if (prev_cell) prev_cell->next = c;
            else r->cells = c;
            prev_cell = c;
        }
        r->next = NULL;
        if (prev) prev->next = r;
        else t->row_data = r;
        prev = r;
    }
    t->next = tables;
    tables = t;
}

struct cell *get_cell(struct table *t, int row, int col) {
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return NULL;
    struct row *r = t->row_data;
    for (int i = 0; i < row; i++) r = r->next;
    struct cell *c = r->cells;
    for (int i = 0; i < col; i++) c = c->next;
    return c;
}

void set_cell(char *name, int row, int col, char *value) {
    struct table *t = find_table(name);
    if (!t) return;
    struct cell *c = get_cell(t, row, col);
    if (!c) return;
    free(c->value);
    c->value = malloc(strlen(value) + 1);
    strcpy(c->value, value);
}

void get_cell_print(char *name, int row, int col) {
    struct table *t = find_table(name);
    if (!t) return;
    struct cell *c = get_cell(t, row, col);
    if (!c) return;
    if (strlen(c->value) == 0) printf("EMPTY\n");
    else printf("%s\n", c->value);
}

void delete_table(char *name) {
    struct table *t = tables, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            free(t->name);
            // free rows and cells
            struct row *r = t->row_data;
            while (r) {
                struct row *next_r = r->next;
                struct cell *c = r->cells;
                while (c) {
                    struct cell *next_c = c->next;
                    free(c->value);
                    free(c);
                    c = next_c;
                }
                free(r);
                r = next_r;
            }
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void list_tables() {
    struct table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], value[4096];
        int rows, cols, row, col;
        int n = sscanf(line, "%31s %255s %d %d %d %d %4095[^\n]", cmd, name, &rows, &cols, &row, &col, value);
        if (strcmp(cmd, "TABLE") == 0 && n >= 4) {
            create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0 && n >= 6) {
            // SET name row col value
            set_cell(name, row, col, value);
        } else if (strcmp(cmd, "GET") == 0 && n >= 5) {
            get_cell_print(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    return 0;
}