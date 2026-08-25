// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARAS 1000
#define MAX_LEN 1000

typedef struct {
    char id[100];
    char text[MAX_LEN];
} Paragraph;

Paragraph paras[MAX_PARAS];
int count = 0;

int find_id(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(paras[i].id, id) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], id[100], text[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%s", id);
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            int idx = find_id(id);
            if (idx == -1 && count < MAX_PARAS) {
                strcpy(paras[count].id, id);
                strcpy(paras[count].text, text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%s", id);
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            int idx = find_id(id);
            if (idx != -1) {
                strcat(paras[idx].text, " ");
                strcat(paras[idx].text, text);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            scanf("%s", id);
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            int idx = find_id(id);
            if (idx != -1) strcpy(paras[idx].text, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) paras[i] = paras[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                for (char *p = paras[idx].text; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            } else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) printf("%s\n", paras[i].id);
        }
    }
    return 0;
}