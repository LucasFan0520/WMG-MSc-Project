// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells;
    struct Table *next;
} Table;

Table *head = NULL;

int main() {
    char cmd[20];
    char name[100];
    char value[1000];
    int rows, cols, r, c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "TABLE")) {
            scanf("%s %d %d", name, &rows, &cols);
            Table *t = malloc(sizeof(Table));
            t->name = strdup(name);
            t->rows = rows;
            t->cols = cols;
            t->cells = malloc(rows * sizeof(char**));
            for (int i = 0; i < rows; i++) {
                t->cells[i] = malloc(cols * sizeof(char*));
                for (int j = 0; j < cols; j++) {
                    t->cells[i][j] = NULL;
                }
            }
            t->next = head;
            head = t;
        } else if (!strcmp(cmd, "SET")) {
            scanf("%s %d %d", name, &r, &c);
            fgets(value, 1000, stdin);
            value[strcspn(value, "\n")] = 0;
            if (value[0] == ' ') memmove(value, value + 1, strlen(value));
            for (Table *t = head; t; t = t->next) {
                if (!strcmp(t->name, name)) {
                    if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                        free(t->cells[r][c]);
                        t->cells[r][c] = strdup(value);
                    }
                    break;
                }
            }
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s %d %d", name, &r, &c);
            for (Table *t = head; t; t = t->next) {
                if (!strcmp(t->name, name)) {
                    if (r >= 0 && r < t->rows && c >= 0 && c < t->cols && t->cells[r][c]) {
                        printf("%s\n", t->cells[r][c]);
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            Table *p = head;
            Table *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    for (int i = 0; i < p->rows; i++) {
                        for (int j = 0; j < p->cols; j++) {
                            free(p->cells[i][j]);
                        }
                        free(p->cells[i]);
                    }
                    free(p->cells);
                    free(p->name);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "TABLES")) {
            for (Table *t = head; t; t = t->next) {
                printf("%s\n", t->name);
            }
        }
    }
    return 0;
}