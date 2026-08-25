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

void create_table(const char *name, int rows, int cols) {
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = NULL;
        }
    }
    t->next = head;
    head = t;
}

void set_cell(const char *name, int r, int c, const char *val) {
    for (Table *t = head; t; t = t->next) {
        if (strcmp(t->name, name) == 0) {
            if (r < 0 || r >= t->rows || c < 0 || c >= t->cols) return;
            free(t->cells[r][c]);
            t->cells[r][c] = strdup(val);
            return;
        }
    }
}

void get_cell(const char *name, int r, int c) {
    for (Table *t = head; t; t = t->next) {
        if (strcmp(t->name, name) == 0) {
            if (r < 0 || r >= t->rows || c < 0 || c >= t->cols) {
                printf("EMPTY\n");
            } else if (t->cells[r][c]) {
                printf("%s\n", t->cells[r][c]);
            } else {
                printf("EMPTY\n");
            }
            return;
        }
    }
}

void delete_table(const char *name) {
    Table *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            for (int i = 0; i < p->rows; i++) {
                for (int j = 0; j < p->cols; j++) {
                    free(p->cells[i][j]);
                }
                free(p->cells[i]);
            }
            free(p->cells);
            free(p->name);
            free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void list_tables() {
    for (Table *t = head; t; t = t->next) {
        printf("%s\n", t->name);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000];
        int r, c;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "TABLE") == 0) {
            sscanf(line, "%*s %s %d %d", a, &r, &c);
            create_table(a, r, c);
        } else if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s %d %d %s", a, &r, &c, b);
            set_cell(a, r, c, b);
        } else if (strcmp(cmd, "GET") == 0) {
            sscanf(line, "%*s %s %d %d", a, &r, &c);
            get_cell(a, r, c);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_table(a);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    while (head) {
        Table *n = head->next;
        for (int i = 0; i < head->rows; i++) {
            for (int j = 0; j < head->cols; j++) {
                free(head->cells[i][j]);
            }
            free(head->cells[i]);
        }
        free(head->cells);
        free(head->name);
        free(head);
        head = n;
    }
    return 0;
}