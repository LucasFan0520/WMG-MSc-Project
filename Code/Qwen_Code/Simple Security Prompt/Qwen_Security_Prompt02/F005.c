// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0;

void show_line(const char *l) {
    for (int i = 0; l[i]; i++) {
        if (l[i] == ' ') {
            putchar('_');
        } else {
            putchar(l[i]);
        }
    }
    putchar('\n');
}

int main() {
    char cmd[20];
    char text[10000];
    int idx;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d", &idx);
            fgets(text, sizeof(text), stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            text[strcspn(text, "\n")] = 0;
            if (idx < 0 || idx > count) {
                continue;
            }
            lines = realloc(lines, sizeof(char*) * (count + 1));
            for (int i = count; i > idx; i--) {
                lines[i] = lines[i - 1];
            }
            lines[idx] = strdup(text);
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            fgets(text, sizeof(text), stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            text[strcspn(text, "\n")] = 0;
            lines = realloc(lines, sizeof(char*) * (count + 1));
            lines[count++] = strdup(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d", &idx);
            if (idx < 0 || idx >= count) {
                continue;
            }
            free(lines[idx]);
            for (int i = idx; i < count - 1; i++) {
                lines[i] = lines[i + 1];
            }
            count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            scanf("%d", &idx);
            fgets(text, sizeof(text), stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            text[strcspn(text, "\n")] = 0;
            if (idx < 0 || idx >= count) {
                continue;
            }
            free(lines[idx]);
            lines[idx] = strdup(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                show_line(lines[i]);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);

    return 0;
}