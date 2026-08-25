// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} StringTable;

StringTable *tables = NULL;
int count = 0;
int capacity = 0;

int find_table(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *p1 = line + 6;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    int rows = atoi(p2);
                    int cols = atoi(sp2 + 1);
                    if (rows > 0 && cols > 0) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            tables = realloc(tables, capacity * sizeof(StringTable));
                        }
                        tables[count].name = strdup(p1);
                        tables[count].rows = rows;
                        tables[count].cols = cols;
                        tables[count].cells = malloc(rows * sizeof(char **));
                        for (int r = 0; r < rows; r++) {
                            tables[count].cells[r] = malloc(cols * sizeof(char *));
                            for (int c = 0; c < cols; c++) {
                                tables[count].cells[r][c] = strdup("EMPTY");
                            }
                        }
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p1 = line + 4;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *p3 = sp2 + 1;
                    char *sp3 = strchr(p3, ' ');
                    if (sp3) {
                        *sp3 = 0;
                        char *value = sp3 + 1;
                        int idx = find_table(p1);
                        if (idx != -1) {
                            int r = atoi(p2);
                            int c = atoi(p3);
                            StringTable *t = &tables[idx];
                            if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                                free(t->cells[r][c]);
                                t->cells[r][c] = strdup(value);
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p1 = line + 4;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    int r = atoi(p2);
                    int c = atoi(sp2 + 1);
                    int idx = find_table(p1);
                    if (idx != -1) {
                        StringTable *t = &tables[idx];
                        if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                            printf("%s\n", t->cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_table(name);
            if (idx != -1) {
                free(tables[idx].name);
                for (int r = 0; r < tables[idx].rows; r++) {
                    for (int c = 0; c < tables[idx].cols; c++) {
                        free(tables[idx].cells[r][c]);
                    }
                    free(tables[idx].cells[r]);
                }
                free(tables[idx].cells);
                for (int i = idx; i < count - 1; i++) {
                    tables[i] = tables[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
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
