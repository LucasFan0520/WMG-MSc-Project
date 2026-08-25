/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows, cols;
    char ***cells; // [rows][cols]
} Table;

static Table *tables = NULL;
static size_t tcount = 0, tcap = 0;

static Table *find_table(const char *name) {
    for (size_t i = 0; i < tcount; i++) {
        if (strcmp(tables[i].name, name) == 0)
            return &tables[i];
    }
    return NULL;
}

static void create_table(const char *name, int rows, int cols) {
    if (rows <= 0 || cols <= 0) return;
    if (find_table(name)) return;
    if (tcount == tcap) {
        tcap = tcap ? tcap * 2 : 4;
        tables = realloc(tables, tcap * sizeof(Table));
        if (!tables) { perror("realloc"); exit(1); }
    }
    Table *t = &tables[tcount];
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    if (!t->cells) { perror("malloc"); exit(1); }
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        if (!t->cells[i]) { perror("malloc"); exit(1); }
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = strdup("");
        }
    }
    tcount++;
}

static void set_cell(Table *t, int row, int col, const char *value) {
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    free(t->cells[row][col]);
    t->cells[row][col] = strdup(value);
}

static void get_cell(Table *t, int row, int col) {
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    printf("%s\n", t->cells[row][col]);
}

static void delete_table(const char *name) {
    for (size_t i = 0; i < tcount; i++) {
        if (strcmp(tables[i].name, name) == 0) {
            free(tables[i].name);
            for (int r = 0; r < tables[i].rows; r++) {
                for (int c = 0; c < tables[i].cols; c++) {
                    free(tables[i].cells[r][c]);
                }
                free(tables[i].cells[r]);
            }
            free(tables[i].cells);
            for (size_t j = i; j < tcount - 1; j++)
                tables[j] = tables[j + 1];
            tcount--;
            return;
        }
    }
}

static void list_tables(void) {
    for (size_t i = 0; i < tcount; i++) {
        printf("%s\n", tables[i].name);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *rstr = strtok(NULL, " ");
            char *cstr = strtok(NULL, " ");
            if (!rstr || !cstr) continue;
            int r = atoi(rstr), c = atoi(cstr);
            create_table(name, r, c);
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *rstr = strtok(NULL, " ");
            char *cstr = strtok(NULL, " ");
            if (!rstr || !cstr) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            Table *t = find_table(name);
            if (t) set_cell(t, atoi(rstr), atoi(cstr), val);
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *rstr = strtok(NULL, " ");
            char *cstr = strtok(NULL, " ");
            if (!rstr || !cstr) continue;
            Table *t = find_table(name);
            if (t) get_cell(t, atoi(rstr), atoi(cstr));
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    for (size_t i = 0; i < tcount; i++) {
        free(tables[i].name);
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++) {
                free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}