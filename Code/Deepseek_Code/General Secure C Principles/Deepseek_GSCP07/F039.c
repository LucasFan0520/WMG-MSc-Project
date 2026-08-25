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

Table *head = NULL;

Table* find_table(const char *name) {
    Table *t = head;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void free_table(Table *t) {
    free(t->name);
    for (int i = 0; i < t->rows; i++) {
        for (int j = 0; j < t->cols; j++) {
            free(t->cells[i][j]);
        }
        free(t->cells[i]);
    }
    free(t->cells);
    free(t);
}

void handle_table(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *rowstr = strtok(NULL, " ");
    char *colstr = strtok(NULL, " ");
    if (!rowstr || !colstr) return;
    int rows = atoi(rowstr);
    int cols = atoi(colstr);
    if (rows <= 0 || cols <= 0) return;
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char**));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char*));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = strdup("");
        }
    }
    t->next = NULL;
    if (!head) head = t;
    else {
        Table *last = head;
        while (last->next) last = last->next;
        last->next = t;
    }
}

void handle_set(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Table *t = find_table(name);
    if (!t) return;
    char *rowstr = strtok(NULL, " ");
    char *colstr = strtok(NULL, " ");
    char *value = strtok(NULL, "");
    if (!rowstr || !colstr) return;
    int row = atoi(rowstr);
    int col = atoi(colstr);
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (!value) value = "";
    free(t->cells[row][col]);
    t->cells[row][col] = strdup(value);
}

void handle_get(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Table *t = find_table(name);
    if (!t) return;
    char *rowstr = strtok(NULL, " ");
    char *colstr = strtok(NULL, " ");
    if (!rowstr || !colstr) return;
    int row = atoi(rowstr);
    int col = atoi(colstr);
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (strlen(t->cells[row][col]) == 0) {
        printf("EMPTY\n");
    } else {
        printf("%s\n", t->cells[row][col]);
    }
}

void handle_delete_table(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Table *t = head, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else head = t->next;
            free_table(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void handle_tables() {
    Table *t = head;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "TABLE") == 0) handle_table(cmd);
        else if (strcmp(cmd, "SET") == 0) handle_set(cmd);
        else if (strcmp(cmd, "GET") == 0) handle_get(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete_table(cmd);
        else if (strcmp(cmd, "TABLES") == 0) handle_tables();
    }
    while (head) {
        Table *tmp = head;
        head = head->next;
        free_table(tmp);
    }
    return 0;
}