// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char **cells;
    int rows;
    int cols;
} Table;

typedef struct {
    char *name;
    Table *tbl;
    struct TableEntry *next;
} TableEntry;

typedef struct {
    TableEntry *head;
    size_t count;
} TableList;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static Table *table_create(int rows, int cols) {
    if (rows <= 0 || cols <= 0) return NULL;
    Table *t = malloc(sizeof(Table));
    if (!t) return NULL;
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char *));
    if (!t->cells) { free(t); return NULL; }
    for (int i = 0; i < rows; i++) {
        t->cells[i] = calloc(cols, sizeof(char *));
        if (!t->cells[i]) {
            for (int j = 0; j < i; j++) free(t->cells[j]);
            free(t->cells);
            free(t);
            return NULL;
        }
    }
    return t;
}

static void table_free(Table *t) {
    if (!t) return;
    for (int i = 0; i < t->rows; i++) {
        for (int j = 0; j < t->cols; j++)
            free(t->cells[i][j]);
        free(t->cells[i]);
    }
    free(t->cells);
    free(t);
}

static void tl_init(TableList *tl) {
    tl->head = NULL;
    tl->count = 0;
}

static void tl_free(TableList *tl) {
    TableEntry *cur = tl->head;
    while (cur) {
        TableEntry *next = cur->next;
        free(cur->name);
        table_free(cur->tbl);
        free(cur);
        cur = next;
    }
    tl->head = NULL;
    tl->count = 0;
}

static TableEntry *tl_find(TableList *tl, const char *name) {
    TableEntry *cur = tl->head;
    while (cur) {
        if (strcmp(cur->name, name) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void tl_table(TableList *tl, const char *name, int rows, int cols) {
    if (tl_find(tl, name)) return;
    Table *t = table_create(rows, cols);
    if (!t) return;
    TableEntry *e = malloc(sizeof(TableEntry));
    if (!e) { table_free(t); return; }
    e->name = safe_strdup(name);
    e->tbl = t;
    e->next = tl->head;
    if (e->name) {
        tl->head = e;
        tl->count++;
    } else {
        free(e);
        table_free(t);
    }
}

static void tl_set(TableList *tl, const char *name, int row, int col, const char *value) {
    TableEntry *e = tl_find(tl, name);
    if (!e) return;
    Table *t = e->tbl;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    char *new = safe_strdup(value);
    if (!new) return;
    free(t->cells[row][col]);
    t->cells[row][col] = new;
}

static void tl_get(TableList *tl, const char *name, int row, int col) {
    TableEntry *e = tl_find(tl, name);
    if (!e) { printf("EMPTY\n"); return; }
    Table *t = e->tbl;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) { printf("EMPTY\n"); return; }
    if (t->cells[row][col])
        printf("%s\n", t->cells[row][col]);
    else
        printf("EMPTY\n");
}

static void tl_delete(TableList *tl, const char *name) {
    TableEntry **ptr = &tl->head;
    while (*ptr) {
        if (strcmp((*ptr)->name, name) == 0) {
            TableEntry *del = *ptr;
            *ptr = del->next;
            free(del->name);
            table_free(del->tbl);
            free(del);
            tl->count--;
            return;
        }
        ptr = &(*ptr)->next;
    }
}

static void tl_tables(TableList *tl) {
    TableEntry *cur = tl->head;
    while (cur) {
        printf("%s\n", cur->name);
        cur = cur->next;
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz, char *a4, size_t a4sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a4sz - 1) a4[i++] = *p++;
    a4[i] = '\0';
}

int main(void) {
    TableList tl;
    tl_init(&tl);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[64], a3[64], a4[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3), a4, sizeof(a4));
        if (strcmp(cmd, "TABLE") == 0) {
            int rows = atoi(a2);
            int cols = atoi(a3);
            tl_table(&tl, a1, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            int row = atoi(a2);
            int col = atoi(a3);
            tl_set(&tl, a1, row, col, a4);
        } else if (strcmp(cmd, "GET") == 0) {
            int row = atoi(a2);
            int col = atoi(a3);
            tl_get(&tl, a1, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            tl_delete(&tl, a1);
        } else if (strcmp(cmd, "TABLES") == 0) {
            tl_tables(&tl);
        }
    }
    tl_free(&tl);
    return 0;
}