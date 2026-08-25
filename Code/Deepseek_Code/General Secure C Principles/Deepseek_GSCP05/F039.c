// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells; // cells[row][col]
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
            // cleanup previous
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
    t->next = tables;
    tables = t;
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
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    char *val = t->cells[row][col];
    if (val && strlen(val) > 0) printf("%s\n", val);
    else printf("EMPTY\n");
}

void delete_table(const char *name) {
    Table *prev = NULL, *cur = tables;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else tables = cur->next;
            for (int i = 0; i < cur->rows; i++) {
                for (int j = 0; j < cur->cols; j++) {
                    free(cur->cells[i][j]);
                }
                free(cur->cells[i]);
            }
            free(cur->cells);
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_tables(void) {
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void free_all(void) {
    Table *t = tables;
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            int rows, cols;
            if (sscanf(space + 1, "%d %d", &rows, &cols) != 2) continue;
            create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            int row, col;
            char *rest = space + 1;
            while (*rest == ' ') rest++;
            if (sscanf(rest, "%d %d", &row, &col) != 2) continue;
            char *val_start = strchr(rest, ' ');
            if (!val_start) continue;
            val_start = strchr(val_start + 1, ' ');
            if (!val_start) continue;
            while (*val_start == ' ') val_start++;
            set_cell(name, row, col, val_start);
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            int row, col;
            if (sscanf(space + 1, "%d %d", &row, &col) != 2) continue;
            get_cell(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}