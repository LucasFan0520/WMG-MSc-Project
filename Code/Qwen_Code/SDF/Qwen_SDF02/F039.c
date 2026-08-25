// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells;
    struct Table *next;
} Table;

Table *tables = NULL;

Table* find_table(const char *name) {
    for (Table *t = tables; t; t = t->next)
        if (strcmp(t->name, name) == 0) return t;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int rows, cols;
            if (sscanf(rest, "%255s %d %d", name, &rows, &cols) != 3) continue;
            if (rows <= 0 || cols <= 0 || find_table(name)) continue;
            Table *t = malloc(sizeof(Table));
            if (!t) continue;
            t->name = strdup(name);
            if (!t->name) { free(t); continue; }
            t->rows = rows;
            t->cols = cols;
            t->cells = malloc(rows * sizeof(char**));
            if (!t->cells) { free(t->name); free(t); continue; }
            for (int i = 0; i < rows; i++) {
                t->cells[i] = calloc(cols, sizeof(char*));
                if (!t->cells[i]) {
                    for (int j = 0; j < i; j++) free(t->cells[j]);
                    free(t->cells); free(t->name); free(t);
                    t = NULL; break;
                }
            }
            if (t) {
                t->next = tables;
                tables = t;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256], value[256];
            int row, col;
            if (sscanf(rest, "%255s %d %d %255s", name, &row, &col, value) != 4) continue;
            Table *t = find_table(name);
            if (t && row >= 0 && row < t->rows && col >= 0 && col < t->cols) {
                free(t->cells[row][col]);
                t->cells[row][col] = strdup(value);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int row, col;
            if (sscanf(rest, "%255s %d %d", name, &row, &col) != 3) continue;
            Table *t = find_table(name);
            if (t && row >= 0 && row < t->rows && col >= 0 && col < t->cols) {
                if (t->cells[row][col]) printf("%s\n", t->cells[row][col]);
                else printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Table *prev = NULL;
            for (Table *t = tables; t; prev = t, t = t->next) {
                if (strcmp(t->name, name) == 0) {
                    if (prev) prev->next = t->next;
                    else tables = t->next;
                    for (int i = 0; i < t->rows; i++) {
                        for (int j = 0; j < t->cols; j++) free(t->cells[i][j]);
                        free(t->cells[i]);
                    }
                    free(t->cells); free(t->name); free(t);
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (Table *t = tables; t; t = t->next) {
                printf("%s\n", t->name);
            }
        }
    }
    while (tables) {
        Table *t = tables;
        tables = tables->next;
        for (int i = 0; i < t->rows; i++) {
            for (int j = 0; j < t->cols; j++) free(t->cells[i][j]);
            free(t->cells[i]);
        }
        free(t->cells); free(t->name); free(t);
    }
    return 0;
}