// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; char *text; } Para;
Para p[1000]; int np = 0;

int find(char *id) {
    for (int i = 0; i < np; i++) if (!strcmp(p[i].id, id)) return i;
    return -1;
}

int main() {
    char cmd[20], id[100], text[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "NEW") || !strcmp(cmd, "APPEND") || !strcmp(cmd, "REPLACE")) {
            scanf("%s", id); fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            int idx = find(id);
            if (!strcmp(cmd, "NEW")) {
                if (idx == -1) { p[np].id = strdup(id); p[np++].text = strdup(text); }
            } else if (!strcmp(cmd, "APPEND")) {
                if (idx != -1) {
                    char *nt = malloc(strlen(p[idx].text) + strlen(text) + 2);
                    sprintf(nt, "%s %s", p[idx].text, text);
                    free(p[idx].text); p[idx].text = nt;
                }
            } else if (!strcmp(cmd, "REPLACE")) {
                if (idx != -1) { free(p[idx].text); p[idx].text = strdup(text); }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", id); int idx = find(id);
            if (idx != -1) {
                free(p[idx].id); free(p[idx].text);
                for (int i = idx; i < np - 1; i++) p[i] = p[i + 1];
                np--;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", id); int idx = find(id);
            if (idx == -1) printf("NOT_FOUND\n");
            else {
                for (char *c = p[idx].text; *c; c++) printf("%c", *c == ' ' ? '_' : *c);
                printf("\n");
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < np; i++) printf("%s\n", p[i].id);
        }
    }
    for (int i = 0; i < np; i++) { free(p[i].id); free(p[i].text); }
    return 0;
}