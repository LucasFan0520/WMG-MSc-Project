// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct {
    char *name;
    size_t rows, cols;
    char ***cells;
} Table;

static Table *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Table *t = realloc(arr, nc * sizeof(Table));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_table(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_table(Table *t) {
    for (size_t i = 0; i < t->rows; i++) {
        for (size_t j = 0; j < t->cols; j++) free(t->cells[i][j]);
        free(t->cells[i]);
    }
    free(t->cells);
    free(t->name);
    t->name = NULL; t->cells = NULL; t->rows = 0; t->cols = 0;
}

static void cmd_table(char *name, size_t rows, size_t cols) {
    if (find_table(name) >= 0) return;
    if (rows == 0 || cols == 0) return;
    if (rows > (size_t)-1 / sizeof(char **) || cols > (size_t)-1 / sizeof(char *)) return;
    char ***cells = malloc(rows * sizeof(char **));
    if (!cells) return;
    for (size_t i = 0; i < rows; i++) {
        cells[i] = malloc(cols * sizeof(char *));
        if (!cells[i]) {
            for (size_t j = 0; j < i; j++) free(cells[j]);
            free(cells);
            return;
        }
        for (size_t j = 0; j < cols; j++) cells[i][j] = NULL;
    }
    if (grow() < 0) {
        for (size_t i = 0; i < rows; i++) free(cells[i]);
        free(cells);
        return;
    }
    arr[cnt].name = safe_strdup(name);
    arr[cnt].rows = rows;
    arr[cnt].cols = cols;
    arr[cnt].cells = cells;
    if (!arr[cnt].name) {
        for (size_t i = 0; i < rows; i++) free(cells[i]);
        free(cells);
        return;
    }
    cnt++;
}

static void cmd_set(char *name, size_t row, size_t col, char *value) {
    int idx = find_table(name);
    if (idx < 0) return;
    Table *t = &arr[idx];
    if (row >= t->rows || col >= t->cols) return;
    char *nv = safe_strdup(value);
    if (!nv) return;
    free(t->cells[row][col]);
    t->cells[row][col] = nv;
}

static void cmd_get(char *name, size_t row, size_t col) {
    int idx = find_table(name);
    if (idx < 0) return;
    Table *t = &arr[idx];
    if (row >= t->rows || col >= t->cols) return;
    if (t->cells[row][col]) printf("%s\n", t->cells[row][col]);
    else printf("EMPTY\n");
}

static void cmd_delete(char *name) {
    int idx = find_table(name);
    if (idx < 0) return;
    free_table(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_tables(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s\n", arr[i].name);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_table(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256] = {0}; size_t rows = 0, cols = 0;
            if (sscanf(rest, "%255s %zu %zu", name, &rows, &cols) < 3) continue;
            cmd_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256] = {0}; size_t row = 0, col = 0; int n2 = 0, n3 = 0, n4 = 0;
            if (sscanf(rest, "%255s%n %zu%n %zu%n", name, &n2, &row, &n3, &col, &n4) < 3) continue;
            char *value = rest + n4;
            while (*value == ' ') value++;
            cmd_set(name, row, col, value);
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256] = {0}; size_t row = 0, col = 0;
            if (sscanf(rest, "%255s %zu %zu", name, &row, &col) < 3) continue;
            cmd_get(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_delete(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            cmd_tables();
        }
    }
    cleanup();
    return 0;
}