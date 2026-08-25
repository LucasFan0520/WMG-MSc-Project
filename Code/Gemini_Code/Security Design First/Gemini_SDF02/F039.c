/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} TableRec;

int main(void) {
    TableRec *tables = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "TABLE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int rows = atoi(r_str);
            int cols = atoi(p);

            if (rows > 0 && cols > 0 && rows < 1000 && cols < 1000) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    TableRec *temp = realloc(tables, new_cap * sizeof(TableRec));
                    if (!temp) break;
                    tables = temp;
                    capacity = new_cap;
                }
                char ***cells = malloc((size_t)rows * sizeof(char **));
                if (cells) {
                    for (int i = 0; i < rows; i++) {
                        cells[i] = calloc((size_t)cols, sizeof(char *));
                    }
                    tables[count].name = strdup(name);
                    tables[count].cells = cells;
                    tables[count].rows = rows;
                    tables[count].cols = cols;
                    if (tables[count].name) count++;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *c_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *value = p;

            int r = atoi(r_str);
            int c = atoi(c_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                        char *temp = strdup(value);
                        if (temp) {
                            free(tables[i].cells[r][c]);
                            tables[i].cells[r][c] = temp;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int r = atoi(r_str);
            int c = atoi(p);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols && tables[i].cells[r][c]) {
                        printf("%s\n", tables[i].cells[r][c]);
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (int r = 0; r < tables[i].rows; r++) {
                        for (int c = 0; c < tables[i].cols; c++) {
                            free(tables[i].cells[r][c]);
                        }
                        free(tables[i].cells[r]);
                    }
                    free(tables[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        tables[j] = tables[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
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
