// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id, *txt; } Para;
Para *paras = 0; int n = 0, cap = 0;

int find(char *id) {
    for (int i=0; i<n; i++) if (!strcmp(paras[i].id, id)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], id[256], txt[1700];
        id[0] = txt[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "LIST")) sscanf(line, "%*s %s %[^\n]", id, txt);
        
        if (!strcmp(cmd, "NEW")) {
            if (find(id) < 0) {
                if (n == cap) { cap = cap ? cap*2 : 4; paras = realloc(paras, cap*sizeof(Para)); }
                paras[n].id = strdup(id); paras[n].txt = strdup(txt); n++;
            }
        } else if (!strcmp(cmd, "APPEND")) {
            int i = find(id);
            if (i >= 0) {
                char *t = malloc(strlen(paras[i].txt) + strlen(txt) + 2);
                sprintf(t, "%s %s", paras[i].txt, txt);
                free(paras[i].txt); paras[i].txt = t;
            }
        } else if (!strcmp(cmd, "REPLACE")) {
            int i = find(id);
            if (i >= 0) { free(paras[i].txt); paras[i].txt = strdup(txt); }
        } else if (!strcmp(cmd, "DELETE")) {
            int i = find(id);
            if (i >= 0) {
                free(paras[i].id); free(paras[i].txt);
                for (int j=i; j<n-1; j++) paras[j] = paras[j+1]; n--;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            int i = find(id);
            if (i < 0) puts("NOT_FOUND");
            else { for (char *c = paras[i].txt; *c; c++) putchar(*c == ' ' ? '_' : *c); putchar('\n'); }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i=0; i<n; i++) puts(paras[i].id);
        }
    }
    return 0;
}