// filename: F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells; // rows x cols
    struct Table *next;
} Table;

static Table *tables = NULL;

static void free_table(Table *t) {
    if (t->cells) {
        for (int i = 0; i < t->rows; ++i) {
            for (int j = 0; j < t->cols; ++j) {
                free(t->cells[i][j]);
            }
            free(t->cells[i]);
        }
        free(t->cells);
    }
    free(t->name);
    free(t);
}

static void cleanup(void) {
    while (tables) {
        Table *tmp = tables;
        tables = tables->next;
        free_table(tmp);
    }
}

static Table *find_table(const char *name) {
    for (Table *t = tables; t; t = t->next)
        if (strcmp(t->name, name) == 0)
            return t;
    return NULL;
}

static void table_create(const char *name, int rows, int cols) {
    if (rows <= 0 || cols <= 0) return;
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    if (!t) return;
    t->name = strdup(name);
    if (!t->name) { free(t); return; }
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    if (!t->cells) { free(t->name); free(t); return; }
    for (int i = 0; i < rows; ++i) {
        t->cells[i] = malloc(cols * sizeof(char *));
        if (!t->cells[i]) {
            // clean up previous rows
            for (int j = 0; j < i; ++j) free(t->cells[j]);
            free(t->cells);
            free(t->name);
            free(t);
            return;
        }
        for (int j = 0; j < cols; ++j)
            t->cells[i][j] = strdup("");
    }
    t->next = tables;
    tables = t;
}

static void table_set(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    char *newv = strdup(value);
    if (!newv) return;
    free(t->cells[row][col]);
    t->cells[row][col] = newv;
}

static void table_get(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (strlen(t->cells[row][col]) == 0)
        printf("EMPTY\n");
    else
        printf("%s\n", t->cells[row][col]);
}

static void table_delete(const char *name) {
    Table *prev = NULL;
    for (Table *t = tables; t; t = t->next) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            free_table(t);
            return;
        }
        prev = t;
    }
}

static void list_tables(void) {
    for (Table *t = tables; t; t = t->next)
        printf("%s\n", t->name);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "TABLE") == 0) {
            if (*arg1 && *arg2) {
                int rows = atoi(arg2);
                int cols = atoi(arg3);
                table_create(arg1, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            if (*arg1 && *arg2 && *arg3) {
                char *arg4 = arg3;
                while (*arg4 && *arg4 != ' ') ++arg4;
                if (*arg4) *arg4++ = '\0';
                int row = atoi(arg2);
                int col = atoi(arg3);
                table_set(arg1, row, col, arg4);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (*arg1 && *arg2 && *arg3) {
                int row = atoi(arg2);
                int col = atoi(arg3);
                table_get(arg1, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) table_delete(arg1);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free(line);
    return 0;
}