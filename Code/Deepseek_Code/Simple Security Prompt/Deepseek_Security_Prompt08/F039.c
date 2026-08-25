// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells; // rows x cols, each cell is char* (or NULL)
    struct Table *next;
} Table;

Table *head = NULL, *tail = NULL;

Table *find_table(const char *name) {
    Table *t = head;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void create_table(const char *name, int rows, int cols) {
    if (rows <= 0 || cols <= 0) return;
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = malloc(strlen(name) + 1);
    strcpy(t->name, name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = calloc(cols, sizeof(char *));
    }
    t->next = NULL;
    if (tail) {
        tail->next = t;
        tail = t;
    } else {
        head = tail = t;
    }
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (t->cells[row][col]) {
        free(t->cells[row][col]);
    }
    t->cells[row][col] = malloc(strlen(value) + 1);
    strcpy(t->cells[row][col], value);
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
    Table *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
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

void list_tables(void) {
    Table *t = head;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void free_all(void) {
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = strtok(NULL, " ");
            char *rows = strtok(NULL, " ");
            char *cols = strtok(NULL, " ");
            if (name && rows && cols) create_table(name, atoi(rows), atoi(cols));
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = strtok(NULL, " ");
            char *row = strtok(NULL, " ");
            char *col = strtok(NULL, " ");
            char *value = strtok(NULL, " ");
            if (name && row && col && value) set_cell(name, atoi(row), atoi(col), value);
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = strtok(NULL, " ");
            char *row = strtok(NULL, " ");
            char *col = strtok(NULL, " ");
            if (name && row && col) get_cell(name, atoi(row), atoi(col));
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free(line);
    free_all();
    return 0;
}