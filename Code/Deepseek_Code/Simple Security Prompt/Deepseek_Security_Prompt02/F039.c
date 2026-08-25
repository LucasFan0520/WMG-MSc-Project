/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Cell {
    char *value;
    struct Cell *next;
} Cell;

typedef struct Table {
    char *name;
    int rows;
    int cols;
    Cell **cells; 
    struct Table *next;
} Table;

Table *tables = NULL;

Table *find_table(const char *name) {
    Table *t = tables;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void free_table(Table *t) {
    free(t->name);
    for (int i = 0; i < t->rows * t->cols; i++) {
        if (t->cells[i]) free(t->cells[i]->value);
        free(t->cells[i]);
    }
    free(t->cells);
    free(t);
}

void table_create(const char *name, int rows, int cols) {
    if (rows <= 0 || cols <= 0) return;
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell *));
    t->next = tables;
    tables = t;
}

void table_set(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx]) {
        free(t->cells[idx]->value);
        t->cells[idx]->value = strdup(value);
    } else {
        Cell *c = malloc(sizeof(Cell));
        c->value = strdup(value);
        c->next = NULL;
        t->cells[idx] = c;
    }
}

void table_get(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) {
        printf("EMPTY\n");
        return;
    }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    int idx = row * t->cols + col;
    if (t->cells[idx]) {
        printf("%s\n", t->cells[idx]->value);
    } else {
        printf("EMPTY\n");
    }
}

void table_delete(const char *name) {
    Table *prev = NULL, *curr = tables;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else tables = curr->next;
            free_table(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void table_list(void) {
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];
    int rows, cols, row, col;
    char value[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "TABLE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d %d", name, &rows, &cols) == 3) table_create(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d %d", name, &row, &col) != 3) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            p += (row >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            p += (col >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            table_set(name, row, col, p);
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d %d", name, &row, &col) == 3) table_get(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) table_delete(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            table_list();
        }
    }

    while (tables) {
        Table *tmp = tables;
        tables = tables->next;
        free_table(tmp);
    }
    return 0;
}