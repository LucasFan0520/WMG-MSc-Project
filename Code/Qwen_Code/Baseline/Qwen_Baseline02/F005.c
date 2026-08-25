// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *lines[10000]; int nl = 0;

int main() {
    char cmd[20], text[10000]; int idx;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "INSERT")) {
            scanf("%d", &idx); fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (idx >= 0 && idx <= nl) {
                for (int i = nl; i > idx; i--) lines[i] = lines[i - 1];
                lines[idx] = strdup(text); nl++;
            }
        } else if (!strcmp(cmd, "APPEND")) {
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            lines[nl++] = strdup(text);
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%d", &idx);
            if (idx >= 0 && idx < nl) {
                free(lines[idx]);
                for (int i = idx; i < nl - 1; i++) lines[i] = lines[i + 1];
                nl--;
            }
        } else if (!strcmp(cmd, "PATCH")) {
            scanf("%d", &idx); fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (idx >= 0 && idx < nl) { free(lines[idx]); lines[idx] = strdup(text); }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < nl; i++) {
                for (char *c = lines[i]; *c; c++) printf("%c", *c == ' ' ? '_' : *c);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < nl; i++) free(lines[i]);
    return 0;
}