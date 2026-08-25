// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

Paragraph *paras = NULL;
int para_count = 0;

int find_id(const char *id) {
    for (int i = 0; i < para_count; i++) {
        if (strcmp(paras[i].id, id) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "NEW") == 0) {
            char id[100];
            scanf("%s", id);
            char text[1000] = "";
            scanf(" %[^\n]", text);
            int idx = find_id(id);
            if (idx == -1) {
                paras = realloc(paras, sizeof(Paragraph) * (para_count + 1));
                paras[para_count].id = strdup(id);
                paras[para_count].text = strdup(text);
                para_count++;
            } else {
                free(paras[idx].text);
                paras[idx].text = strdup(text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char id[100];
            scanf("%s", id);
            char text[1000] = "";
            scanf(" %[^\n]", text);
            int idx = find_id(id);
            if (idx != -1) {
                char *new_text = malloc(strlen(paras[idx].text) + strlen(text) + 2);
                strcpy(new_text, paras[idx].text);
                strcat(new_text, " ");
                strcat(new_text, text);
                free(paras[idx].text);
                paras[idx].text = new_text;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char id[100];
            scanf("%s", id);
            char text[1000] = "";
            scanf(" %[^\n]", text);
            int idx = find_id(id);
            if (idx != -1) {
                free(paras[idx].text);
                paras[idx].text = strdup(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[100];
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                free(paras[idx].id);
                free(paras[idx].text);
                for (int i = idx; i < para_count - 1; i++) {
                    paras[i] = paras[i+1];
                }
                para_count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[100];
            scanf("%s", id);
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
            for (int i = 0; i < para_count; i++) {
                printf("%s\n", paras[i].id);
            }
        }
    }
    for (int i = 0; i < para_count; i++) {
        free(paras[i].id);
        free(paras[i].text);
    }
    free(paras);
    return 0;
}