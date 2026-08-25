// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_CELL 256

typedef struct Cell {
    char val[MAX_CELL];
    struct Cell *next;
} Cell;

typedef struct Table {
    char name[MAX_NAME];
    int rows, cols;
    Cell **cells; // rows x cols, row-major
    struct Table *next;
} Table;

Table *head = NULL;

Table *find_table(const char *name) {
    Table *t = head;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void free_table(Table *t) {
    if (t->cells) {
        for (int i = 0; i < t->rows * t->cols; i++) {
            if (t->cells[i]) free(t->cells[i]);
        }
        free(t->cells);
    }
    free(t);
}

void free_all() {
    Table *t = head;
    while (t) {
        Table *tmp = t;
        t = t->next;
        free_table(tmp);
    }
    head = NULL;
}

void handle_table(char *line) {
    char name[MAX_NAME];
    int rows, cols;
    if (sscanf(line, "TABLE %63s %d %d", name, &rows, &cols) != 3) return;
    if (rows <= 0 || cols <= 0) return;
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    if (!t) return;
    strcpy(t->name, name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell *));
    if (!t->cells) { free(t); return; }
    t->next = head;
    head = t;
}

void handle_set(char *line) {
    char name[MAX_NAME];
    int row, col;
    char val[MAX_CELL];
    int n = sscanf(line, "SET %63s %d %d %255[^\n]", name, &row, &col, val);
    if (n < 4) return;
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx]) free(t->cells[idx]);
    Cell *c = malloc(sizeof(Cell));
    if (!c) return;
    strcpy(c->val, val);
    t->cells[idx] = c;
}

void handle_get(char *line) {
    char name[MAX_NAME];
    int row, col;
    if (sscanf(line, "GET %63s %d %d", name, &row, &col) != 3) return;
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    int idx = row * t->cols + col;
    if (t->cells[idx]) printf("%s\n", t->cells[idx]->val);
    else printf("EMPTY\n");
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Table *t = head, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else head = t->next;
            free_table(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void handle_tables() {
    Table *t = head;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "TABLE ", 6) == 0) handle_table(line);
        else if (strncmp(line, "SET ", 4) == 0) handle_set(line);
        else if (strncmp(line, "GET ", 4) == 0) handle_get(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strcmp(line, "TABLES") == 0) handle_tables();
    }
    free_all();
    return 0;
}