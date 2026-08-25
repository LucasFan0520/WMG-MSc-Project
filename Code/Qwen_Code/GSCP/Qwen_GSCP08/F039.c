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

Table *find_table(char *name) {
    for (Table *t = head; t; t = t->next) {
        if (strcmp(t->name, name) == 0) return t;
    }
    return NULL;
}

void create_table(char *name, int rows, int cols) {
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(sizeof(char**) * rows);
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(sizeof(char*) * cols);
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = NULL;
        }
    }
    t->next = head;
    head = t;
}

void set_cell(char *name, int r, int c, char *value) {
    Table *t = find_table(name);
    if (!t || r < 0 || r >= t->rows || c < 0 || c >= t->cols) return;
    free(t->cells[r][c]);
    t->cells[r][c] = strdup(value);
}

void get_cell(char *name, int r, int c) {
    Table *t = find_table(name);
    if (!t || r < 0 || r >= t->rows || c < 0 || c >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    if (t->cells[r][c]) printf("%s\n", t->cells[r][c]);
    else printf("EMPTY\n");
}

void delete_table(char *name) {
    Table *prev = NULL;
    Table *curr = head;
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

void list_tables() {
    for (Table *t = head; t; t = t->next) {
        printf("%s\n", t->name);
    }
}

int main() {
    char cmd[20], a[100], val[1000];
    int r, c;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TABLE") == 0) {
            scanf("%99s%d%d", a, &r, &c);
            create_table(a, r, c);
        } else if (strcmp(cmd, "SET") == 0) {
            scanf("%99s%d%d", a, &r, &c);
            fgets(val, sizeof(val), stdin);
            val[strcspn(val, "\n")] = 0;
            set_cell(a, r, c, val);
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%99s%d%d", a, &r, &c);
            get_cell(a, r, c);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_table(a);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    return 0;
}