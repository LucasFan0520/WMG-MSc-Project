// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tab {
    char *name;
    int rows, cols;
    char ***cells;
    struct Tab *next;
} Tab;

Tab *head = NULL;

int main() {
    char cmd[20], name[256], val[256];
    int r, c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TABLE") == 0) {
            scanf("%s %d %d", name, &r, &c);
            Tab *t = malloc(sizeof(Tab));
            t->name = strdup(name); t->rows = r; t->cols = c;
            t->cells = malloc(r * sizeof(char**));
            for (int i = 0; i < r; i++) {
                t->cells[i] = malloc(c * sizeof(char*));
                for (int j = 0; j < c; j++) t->cells[i][j] = NULL;
            }
            t->next = head; head = t;
        } else if (strcmp(cmd, "SET") == 0) {
            scanf("%s %d %d %s", name, &r, &c, val);
            for (Tab *t = head; t; t = t->next) {
                if (strcmp(t->name, name) == 0) {
                    if (r >= 0 && r < t->rows && c >= 0 && c < t->cols) {
                        free(t->cells[r][c]);
                        t->cells[r][c] = strdup(val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s %d %d", name, &r, &c);
            for (Tab *t = head; t; t = t->next) {
                if (strcmp(t->name, name) == 0) {
                    if (r >= 0 && r < t->rows && c >= 0 && c < t->cols && t->cells[r][c]) {
                        printf("%s\n", t->cells[r][c]);
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            Tab *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    for (int i = 0; i < curr->rows; i++) {
                        for (int j = 0; j < curr->cols; j++) free(curr->cells[i][j]);
                        free(curr->cells[i]);
                    }
                    free(curr->cells); free(curr->name); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (Tab *t = head; t; t = t->next) printf("%s\n", t->name);
        }
    }
    return 0;
}