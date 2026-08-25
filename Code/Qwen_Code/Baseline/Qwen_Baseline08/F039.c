// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TBL 100
#define MAX_CELLS 100000
#define MAX_LEN 100

typedef struct {
    char name[100];
    int rows;
    int cols;
    char cells[MAX_CELLS][MAX_LEN];
} Tbl;

Tbl tbls[MAX_TBL];
int tcount = 0;

int find_tbl(const char *name) {
    for (int i = 0; i < tcount; i++) if (strcmp(tbls[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], val[MAX_LEN];
    int rows, cols, r, c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TABLE") == 0) {
            scanf("%s %d %d", name, &rows, &cols);
            if (find_tbl(name) == -1 && tcount < MAX_TBL) {
                strcpy(tbls[tcount].name, name);
                tbls[tcount].rows = rows;
                tbls[tcount].cols = cols;
                for (int i = 0; i < rows * cols && i < MAX_CELLS; i++) {
                    tbls[tcount].cells[i][0] = '\0';
                }
                tcount++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            scanf("%s %d %d", name, &r, &c);
            fgets(val, MAX_LEN, stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find_tbl(name);
            if (idx != -1 && r >= 0 && r < tbls[idx].rows && c >= 0 && c < tbls[idx].cols) {
                int pos = r * tbls[idx].cols + c;
                if (pos < MAX_CELLS) strcpy(tbls[idx].cells[pos], val);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s %d %d", name, &r, &c);
            int idx = find_tbl(name);
            if (idx != -1 && r >= 0 && r < tbls[idx].rows && c >= 0 && c < tbls[idx].cols) {
                int pos = r * tbls[idx].cols + c;
                if (pos < MAX_CELLS && tbls[idx].cells[pos][0] != '\0') {
                    printf("%s\n", tbls[idx].cells[pos]);
                } else {
                    printf("EMPTY\n");
                }
            } else {
                printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_tbl(name);
            if (idx != -1) {
                for (int i = idx; i < tcount - 1; i++) tbls[i] = tbls[i + 1];
                tcount--;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < tcount; i++) {
                printf("%s\n", tbls[i].name);
            }
        }
    }
    return 0;
}