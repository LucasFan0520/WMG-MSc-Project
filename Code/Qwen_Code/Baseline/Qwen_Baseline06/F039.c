// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char ***cells; int rows, cols; } Tab;
Tab *tabs; int n_tab;

int find_tab(char *name) {
    for(int i=0; i<n_tab; i++) if(!strcmp(tabs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], val[100]; int rows, cols, r, c;
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "TABLE")) {
            scanf("%s %d %d", name, &rows, &cols);
            tabs = realloc(tabs, (n_tab+1)*sizeof(Tab));
            tabs[n_tab].name = strdup(name);
            tabs[n_tab].rows = rows;
            tabs[n_tab].cols = cols;
            tabs[n_tab].cells = malloc(rows * sizeof(char**));
            for(int i=0; i<rows; i++) {
                tabs[n_tab].cells[i] = malloc(cols * sizeof(char*));
                for(int j=0; j<cols; j++) tabs[n_tab].cells[i][j] = NULL;
            }
            n_tab++;
        } else if(!strcmp(cmd, "SET")) {
            scanf("%s %d %d %s", name, &r, &c, val);
            int idx = find_tab(name);
            if(idx != -1 && r >= 0 && r < tabs[idx].rows && c >= 0 && c < tabs[idx].cols) {
                if(tabs[idx].cells[r][c]) free(tabs[idx].cells[r][c]);
                tabs[idx].cells[r][c] = strdup(val);
            }
        } else if(!strcmp(cmd, "GET")) {
            scanf("%s %d %d", name, &r, &c);
            int idx = find_tab(name);
            if(idx != -1 && r >= 0 && r < tabs[idx].rows && c >= 0 && c < tabs[idx].cols) {
                if(tabs[idx].cells[r][c]) printf("%s\n", tabs[idx].cells[r][c]);
                else printf("EMPTY\n");
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_tab(name);
            if(idx != -1) {
                free(tabs[idx].name);
                for(int i=0; i<tabs[idx].rows; i++) {
                    for(int j=0; j<tabs[idx].cols; j++) {
                        if(tabs[idx].cells[i][j]) free(tabs[idx].cells[i][j]);
                    }
                    free(tabs[idx].cells[i]);
                }
                free(tabs[idx].cells);
                for(int k=idx; k<n_tab-1; k++) tabs[k] = tabs[k+1];
                n_tab--;
            }
        } else if(!strcmp(cmd, "TABLES")) {
            for(int i=0; i<n_tab; i++) printf("%s\n", tabs[i].name);
        }
    }
    return 0;
}