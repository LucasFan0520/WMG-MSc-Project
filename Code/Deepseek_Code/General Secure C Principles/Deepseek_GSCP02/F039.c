// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Cell {
    char *value;
} Cell;

typedef struct Table {
    char *name;
    int rows;
    int cols;
    Cell **cells; // rows * cols
    struct Table *next;
} Table;

Table *t_head = NULL;

Table *find_table(const char *name) {
    Table *t = t_head;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows <= 0 || cols <= 0) return;
    Table *t = malloc(sizeof(Table));
    if (!t) return;
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * cols * sizeof(Cell));
    if (!t->cells) {
        free(t->name);
        free(t);
        return;
    }
    for (int i = 0; i < rows * cols; i++) {
        t->cells[i].value = strdup("");
    }
    t->next = t_head;
    t_head = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    free(t->cells[idx].value);
    t->cells[idx].value = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) { printf("EMPTY\n"); return; }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    int idx = row * t->cols + col;
    if (strlen(t->cells[idx].value) == 0) {
        printf("EMPTY\n");
    } else {
        for (char *p = t->cells[idx].value; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
    }
}

void delete_table(const char *name) {
    Table *prev = NULL;
    Table *t = t_head;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else t_head = t->next;
            free(t->name);
            for (int i = 0; i < t->rows * t->cols; i++) {
                free(t->cells[i].value);
            }
            free(t->cells);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void list_tables(void) {
    Table *t = t_head;
    while (t) {
        printf("%s %d %d\n", t->name, t->rows, t->cols);
        t = t->next;
    }
}

void free_all(void) {
    while (t_head) {
        Table *next = t_head->next;
        free(t_head->name);
        for (int i = 0; i < t_head->rows * t_head->cols; i++) {
            free(t_head->cells[i].value);
        }
        free(t_head->cells);
        free(t_head);
        t_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int rows, cols;
            if (sscanf(line, "%*s %255s %d %d", name, &rows, &cols) == 3) {
                create_table(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256], value[1024];
            int row, col;
            if (sscanf(line, "%*s %255s %d %d %1023[^\n]", name, &row, &col, value) == 4) {
                set_cell(name, row, col, value);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int row, col;
            if (sscanf(line, "%*s %255s %d %d", name, &row, &col) == 3) {
                get_cell(name, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_table(name);
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }

    free(line);
    free_all();
    return 0;
}