// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

Table tables[1000];
int tc = 0;

int find_name(char *name) {
    for (int i = 0; i < tc; i++) if (strcmp(tables[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "TABLE") == 0) {
            char name[1000];
            int r, c;
            sscanf(rest, "%s %d %d", name, &r, &c);
            if (find_name(name) == -1 && r > 0 && c > 0) {
                tables[tc].name = strdup(name);
                tables[tc].rows = r;
                tables[tc].cols = c;
                tables[tc].cells = malloc(sizeof(char **) * r);
                for (int i = 0; i < r; i++) {
                    tables[tc].cells[i] = malloc(sizeof(char *) * c);
                    for (int j = 0; j < c; j++) {
                        tables[tc].cells[i][j] = NULL;
                    }
                }
                tc++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[1000], val[1000];
            int r, c;
            sscanf(rest, "%s %d %d %s", name, &r, &c, val);
            int idx = find_name(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                if (tables[idx].cells[r][c]) free(tables[idx].cells[r][c]);
                tables[idx].cells[r][c] = strdup(val);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[1000];
            int r, c;
            sscanf(rest, "%s %d %d", name, &r, &c);
            int idx = find_name(name);
            if (idx != -1 && r >= 0 && r < tables[idx].rows && c >= 0 && c < tables[idx].cols) {
                if (tables[idx].cells[r][c]) printf("%s\n", tables[idx].cells[r][c]);
                else printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                free(tables[idx].name);
                for (int i = 0; i < tables[idx].rows; i++) {
                    for (int j = 0; j < tables[idx].cols; j++) {
                        if (tables[idx].cells[i][j]) free(tables[idx].cells[i][j]);
                    }
                    free(tables[idx].cells[i]);
                }
                free(tables[idx].cells);
                for (int i = idx; i < tc - 1; i++) tables[i] = tables[i+1];
                tc--;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < tc; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < tc; i++) {
        free(tables[i].name);
        for (int j = 0; j < tables[i].rows; j++) {
            for (int k = 0; k < tables[i].cols; k++) {
                if (tables[i].cells[j][k]) free(tables[i].cells[j][k]);
            }
            free(tables[i].cells[j]);
        }
        free(tables[i].cells);
    }
    return 0;
}