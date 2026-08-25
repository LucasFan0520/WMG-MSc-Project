// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 10000

char lines[10000][MAX_LINE];
int line_count = 0;

void print_with_underscores(const char *text) {
    for (int i = 0; text[i]; i++) {
        if (text[i] == ' ') putchar('_');
        else putchar(text[i]);
    }
    putchar('\n');
}

int main() {
    char cmd[20], text[MAX_LINE];
    int index;
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d", &index);
            fgets(text, MAX_LINE, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            if (index >= 0 && index <= line_count) {
                for (int i = line_count; i > index; i--) {
                    strcpy(lines[i], lines[i - 1]);
                }
                strcpy(lines[index], text);
                line_count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            fgets(text, MAX_LINE, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            strcpy(lines[line_count], text);
            line_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d", &index);
            if (index >= 0 && index < line_count) {
                for (int i = index; i < line_count - 1; i++) {
                    strcpy(lines[i], lines[i + 1]);
                }
                line_count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            scanf("%d", &index);
            fgets(text, MAX_LINE, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            if (index >= 0 && index < line_count) {
                strcpy(lines[index], text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < line_count; i++) {
                print_with_underscores(lines[i]);
            }
        }
    }
    
    return 0;
}