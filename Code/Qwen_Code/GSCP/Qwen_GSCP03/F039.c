// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows, cols;
    char ***cells;
    struct Table *next;
} Table;

Table *head = NULL;

void free_table(Table *t) {
    for (int i = 0; i < t->rows; i++) {
        for (int j = 0; j < t->cols; j++) {
            if (t->cells[i][j]) free(t->cells[i][j]);
        }
        free(t->cells[i]);
    }
    free(t->cells);
    free(t->name);
    free(t);
}

void free_list() {
    while (head) {
        Table *t = head; head = head->next;
        free_table(t);
    }
}

Table *find_table(const char *name) {
    for (Table *t = head; t; t = t->next)
        if (strcmp(t->name, name) == 0) return t;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], val[4096] = "";
        int r, c, rows, cols;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &rows, &cols) != 3) continue;
            if (find_table(name)) continue;
            Table *t = malloc(sizeof(Table));
            t->name = strdup(name); t->rows = rows; t->cols = cols;
            t->cells = malloc(rows * sizeof(char**));
            for (int i = 0; i < rows; i++) {
                t->cells[i] = calloc(cols, sizeof(char*));
            }
            t->next = head; head = t;
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %4095[^\n]", name, &r, &c, val) < 3) continue;
            Table *t = find_table(name);
            if (t && r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                if (t->cells[r][c]) free(t->cells[r][c]);
                t->cells[r][c] = strdup(val);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &r, &c) != 3) continue;
            Table *t = find_table(name);
            if (t && r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                if (t->cells[r][c]) printf("%s\n", t->cells[r][c]);
                else printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Table *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Table *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free_table(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (Table *t = head; t; t = t->next) {
                printf("%s\n", t->name);
            }
        }
    }
    free_list();
    return 0;
}