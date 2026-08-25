// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Para;

Para paras[1000];
int pcount = 0;

int find_id(char *id) {
    for (int i = 0; i < pcount; i++) {
        if (strcmp(paras[i].id, id) == 0) return i;
    }
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
        
        if (strcmp(cmd, "NEW") == 0) {
            char id[1000];
            sscanf(rest, "%s", id);
            char *text = rest + strlen(id);
            while (*text == ' ') text++;
            if (find_id(id) != -1) continue;
            paras[pcount].id = strdup(id);
            paras[pcount].text = strdup(text);
            pcount++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char id[1000];
            sscanf(rest, "%s", id);
            char *text = rest + strlen(id);
            while (*text == ' ') text++;
            int idx = find_id(id);
            if (idx != -1) {
                char *nt = malloc(strlen(paras[idx].text) + strlen(text) + 2);
                sprintf(nt, "%s %s", paras[idx].text, text);
                free(paras[idx].text);
                paras[idx].text = nt;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char id[1000];
            sscanf(rest, "%s", id);
            char *text = rest + strlen(id);
            while (*text == ' ') text++;
            int idx = find_id(id);
            if (idx != -1) {
                free(paras[idx].text);
                paras[idx].text = strdup(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[1000];
            sscanf(rest, "%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                free(paras[idx].id);
                free(paras[idx].text);
                for (int i = idx; i < pcount - 1; i++) paras[i] = paras[i+1];
                pcount--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[1000];
            sscanf(rest, "%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                for (char *p = paras[idx].text; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < pcount; i++) printf("%s\n", paras[i].id);
        }
    }
    for (int i = 0; i < pcount; i++) {
        free(paras[i].id);
        free(paras[i].text);
    }
    return 0;
}