// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Para;

Para *paras = NULL;
int pcount = 0;

void add_para(const char *id, const char *text) {
    paras = realloc(paras, (pcount + 1) * sizeof(Para));
    paras[pcount].id = strdup(id);
    paras[pcount].text = strdup(text);
    pcount++;
}

int find_para(const char *id) {
    for (int i = 0; i < pcount; i++) {
        if (strcmp(paras[i].id, id) == 0) return i;
    }
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], id[256], text[700];
        text[0] = 0;
        int n = sscanf(line, "%19s %255s %[^\n]", cmd, id, text);
        if (n < 2) continue;
        
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0) {
            int idx = find_para(id);
            if (idx >= 0) {
                char *newtext = malloc(strlen(paras[idx].text) + strlen(text) + 2);
                strcpy(newtext, paras[idx].text);
                if (text[0]) { strcat(newtext, " "); strcat(newtext, text); }
                free(paras[idx].text);
                paras[idx].text = newtext;
            } else {
                add_para(id, text);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            int idx = find_para(id);
            if (idx >= 0) {
                free(paras[idx].text);
                paras[idx].text = strdup(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_para(id);
            if (idx >= 0) {
                free(paras[idx].id); free(paras[idx].text);
                for (int i = idx; i < pcount - 1; i++) paras[i] = paras[i+1];
                pcount--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            int idx = find_para(id);
            if (idx >= 0) {
                for (char *p = paras[idx].text; *p; p++) {
                    putchar(*p == ' ' ? '_' : *p);
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < pcount; i++) printf("%s\n", paras[i].id);
        }
    }
    for (int i = 0; i < pcount; i++) { free(paras[i].id); free(paras[i].text); }
    free(paras);
    return 0;
}