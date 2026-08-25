/* F039.c */
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
    Table *t = head;
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
    t->cells = malloc(rows * sizeof(char **));
    if (!t->cells) { free(t->name); free(t); return; }
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        if (!t->cells[i]) {
            for (int j = 0; j < i; j++) free(t->cells[j]);
            free(t->cells);
            free(t->name);
            free(t);
            return;
        }
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = strdup("");
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
    if (!t) {
        printf("NOT_FOUND\n");
        return;
    }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    if (strlen(t->cells[row][col]) == 0) {
        printf("EMPTY\n");
    } else {
        printf("%s\n", t->cells[row][col]);
    }
}

void delete_table(const char *name) {
    Table *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            for (int i = 0; i < curr->rows; i++) {
                for (int j = 0; j < curr->cols; j++) {
                    free(curr->cells[i][j]);
                }
                free(curr->cells[i]);
            }
            free(curr->cells);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_tables(void) {
    Table *t = head;
    int first = 1;
    while (t) {
        if (!first) printf(" ");
        printf("%s", t->name);
        first = 0;
        t = t->next;
    }
    printf("\n");
}

void free_all(void) {
    Table *t = head;
    while (t) {
        Table *tmp = t;
        t = t->next;
        free(tmp->name);
        for (int i = 0; i < tmp->rows; i++) {
            for (int j = 0; j < tmp->cols; j++) {
                free(tmp->cells[i][j]);
            }
            free(tmp->cells[i]);
        }
        free(tmp->cells);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int rows, cols;
            if (sscanf(line + 6, "%255s %d %d", name, &rows, &cols) == 3) {
                create_table(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256];
            int row, col;
            char value[256];
            if (sscanf(line + 4, "%255s %d %d %255s", name, &row, &col, value) == 4) {
                set_cell(name, row, col, value);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int row, col;
            if (sscanf(line + 4, "%255s %d %d", name, &row, &col) == 3) {
                get_cell(name, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_table(p);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}