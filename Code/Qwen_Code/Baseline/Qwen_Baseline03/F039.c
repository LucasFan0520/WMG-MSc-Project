// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    int rows;
    int cols;
    char **cells;
} Tab;

Tab tabs[1000];
int tn = 0;

int findTab(char *name) {
    for (int i = 0; i < tn; i++) {
        if (!strcmp(tabs[i].name, name)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], val[1000];
    int rows, cols, r, c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "TABLE")) {
            scanf("%s %d %d", name, &rows, &cols);
            strcpy(tabs[tn].name, name);
            tabs[tn].rows = rows;
            tabs[tn].cols = cols;
            tabs[tn].cells = malloc(rows * cols * sizeof(char *));
            for (int i = 0; i < rows * cols; i++) {
                tabs[tn].cells[i] = NULL;
            }
            tn++;
        } else if (!strcmp(cmd, "SET")) {
            scanf("%s %d %d", name, &r, &c);
            fgets(val, 1000, stdin);
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            val[strcspn(val, "\n")] = 0;
            int i = findTab(name);
            if (i != -1 && r >= 0 && r < tabs[i].rows && c >= 0 && c < tabs[i].cols) {
                free(tabs[i].cells[r * tabs[i].cols + c]);
                tabs[i].cells[r * tabs[i].cols + c] = malloc(strlen(val) + 1);
                strcpy(tabs[i].cells[r * tabs[i].cols + c], val);
            }
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s %d %d", name, &r, &c);
            int i = findTab(name);
            if (i != -1 && r >= 0 && r < tabs[i].rows && c >= 0 && c < tabs[i].cols) {
                if (tabs[i].cells[r * tabs[i].cols + c] == NULL) {
                    printf("EMPTY\n");
                } else {
                    printf("%s\n", tabs[i].cells[r * tabs[i].cols + c]);
                }
            } else {
                printf("EMPTY\n");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int i = findTab(name);
            if (i != -1) {
                for (int j = 0; j < tabs[i].rows * tabs[i].cols; j++) {
                    free(tabs[i].cells[j]);
                }
                free(tabs[i].cells);
                for (int j = i; j < tn - 1; j++) {
                    tabs[j] = tabs[j + 1];
                }
                tn--;
            }
        } else if (!strcmp(cmd, "TABLES")) {
            for (int i = 0; i < tn; i++) {
                printf("%s\n", tabs[i].name);
            }
        }
    }
    return 0;
}