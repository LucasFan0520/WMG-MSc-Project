// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

Paragraph *paras = NULL;
int count = 0;

int find_id(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(paras[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

void show_text(const char *t) {
    for (int i = 0; t[i]; i++) {
        if (t[i] == ' ') {
            putchar('_');
        } else {
            putchar(t[i]);
        }
    }
    putchar('\n');
}

int main() {
    char cmd[20];
    char id[1000];
    char text[10000];
    
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            scanf("%s", id);
            fgets(text, sizeof(text), stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            text[strcspn(text, "\n")] = 0;
            
            int idx = find_id(id);
            if (strcmp(cmd, "NEW") == 0) {
                if (idx != -1) {
                    free(paras[idx].text);
                    paras[idx].text = strdup(text);
                } else {
                    paras = realloc(paras, sizeof(Paragraph) * (count + 1));
                    paras[count].id = strdup(id);
                    paras[count].text = strdup(text);
                    count++;
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                if (idx != -1) {
                    char *nt = malloc(strlen(paras[idx].text) + strlen(text) + 2);
                    sprintf(nt, "%s %s", paras[idx].text, text);
                    free(paras[idx].text);
                    paras[idx].text = nt;
                } else {
                    paras = realloc(paras, sizeof(Paragraph) * (count + 1));
                    paras[count].id = strdup(id);
                    paras[count].text = strdup(text);
                    count++;
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                if (idx != -1) {
                    free(paras[idx].text);
                    paras[idx].text = strdup(text);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                free(paras[idx].id);
                free(paras[idx].text);
                for (int i = idx; i < count - 1; i++) {
                    paras[i] = paras[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                show_text(paras[idx].text);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", paras[i].id);
            }
        }
    }
    
    for (int i = 0; i < count; i++) {
        free(paras[i].id);
        free(paras[i].text);
    }
    free(paras);
    
    return 0;
}