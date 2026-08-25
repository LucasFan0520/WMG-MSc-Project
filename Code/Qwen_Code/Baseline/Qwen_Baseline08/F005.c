// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 10000
#define MAX_LEN 1000

char lines[MAX_LINES][MAX_LEN];
int count = 0;

int main() {
    char cmd[20], text[MAX_LEN];
    int idx;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d", &idx);
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (idx >= 0 && idx <= count && count < MAX_LINES) {
                for (int i = count; i > idx; i--) strcpy(lines[i], lines[i - 1]);
                strcpy(lines[idx], text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (count < MAX_LINES) strcpy(lines[count++], text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d", &idx);
            if (idx >= 0 && idx < count) {
                for (int i = idx; i < count - 1; i++) strcpy(lines[i], lines[i + 1]);
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            scanf("%d", &idx);
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (idx >= 0 && idx < count) strcpy(lines[idx], text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                for (char *p = lines[i]; *p; p++) putchar(*p == ' ' ? '_' : *p);
                putchar('\n');
            }
        }
    }
    return 0;
}