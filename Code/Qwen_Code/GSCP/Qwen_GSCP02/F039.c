/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

static Table *tables = NULL;
static int tcount = 0;
static int tcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_table(const char *name) {
    for (int i = 0; i < tcount; i++)
        if (strcmp(tables[i].name, name) == 0) return i;
    return -1;
}

static void free_table(Table *t) {
    for (int i = 0; i < t->rows; i++) {
        for (int j = 0; j < t->cols; j++) {
            free(t->cells[i][j]);
        }
        free(t->cells[i]);
    }
    free(t->cells);
    free(t->name);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "TABLE") == 0) {
            char name[256] = {0};
            int rows = 0, cols = 0;
            if (sscanf(line + offset, "%255s %d %d", name, &rows, &cols) != 3) continue;
            if (rows < 0 || cols < 0) continue;
            if (find_table(name) >= 0) continue;
            if (tcount >= tcap) {
                tcap = tcap == 0 ? 8 : tcap * 2;
                tables = realloc(tables, sizeof(Table) * tcap);
            }
            tables[tcount].name = dup_str(name);
            tables[tcount].rows = rows;
            tables[tcount].cols = cols;
            tables[tcount].cells = malloc(sizeof(char **) * rows);
            if (!tables[tcount].cells && rows > 0) continue;
            for (int i = 0; i < rows; i++) {
                tables[tcount].cells[i] = malloc(sizeof(char *) * cols);
                if (!tables[tcount].cells[i] && cols > 0) continue;
                for (int j = 0; j < cols; j++) {
                    tables[tcount].cells[i][j] = NULL;
                }
            }
            tcount++;
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256] = {0};
            int r = 0, c = 0;
            int off2 = 0, off3 = 0, off4 = 0;
            if (sscanf(line + offset, "%255s%n %d%n %d%n", name, &off2, &r, &off3, &c, &off4) != 3) continue;
            const char *val = line + offset + off2 + off3 + off4;
            while (*val == ' ') val++;
            int idx = find_table(name);
            if (idx < 0 || r < 0 || r >= tables[idx].rows || c < 0 || c >= tables[idx].cols) continue;
            free(tables[idx].cells[r][c]);
            tables[idx].cells[r][c] = dup_str(val);
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256] = {0};
            int r = 0, c = 0;
            if (sscanf(line + offset, "%255s %d %d", name, &r, &c) != 3) continue;
            int idx = find_table(name);
            if (idx < 0 || r < 0 || r >= tables[idx].rows || c < 0 || c >= tables[idx].cols) {
                printf("EMPTY\n");
                continue;
            }
            if (tables[idx].cells[r][c]) printf("%s\n", tables[idx].cells[r][c]);
            else printf("EMPTY\n");
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_table(name);
            if (idx < 0) continue;
            free_table(&tables[idx]);
            for (int i = idx; i < tcount - 1; i++) tables[i] = tables[i + 1];
            tcount--;
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < tcount; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < tcount; i++) {
        free_table(&tables[i]);
    }
    free(tables);
    return 0;
}