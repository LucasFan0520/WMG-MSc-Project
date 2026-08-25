// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID 100
#define MAX_TEXT 10000

typedef struct {
    char id[MAX_ID];
    char text[MAX_TEXT];
} Paragraph;

Paragraph paragraphs[1000];
int count = 0;

int find_paragraph(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(paragraphs[i].id, id) == 0) return i;
    }
    return -1;
}

void print_with_underscores(const char *text) {
    for (int i = 0; text[i]; i++) {
        if (text[i] == ' ') putchar('_');
        else putchar(text[i]);
    }
    putchar('\n');
}

int main() {
    char cmd[20], id[MAX_ID], text[MAX_TEXT];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%s", id);
            fgets(text, MAX_TEXT, stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_paragraph(id);
            if (idx == -1) {
                strcpy(paragraphs[count].id, id);
                strcpy(paragraphs[count].text, text);
                count++;
            } else {
                strcpy(paragraphs[idx].text, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%s", id);
            fgets(text, MAX_TEXT, stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_paragraph(id);
            if (idx != -1) {
                strcat(paragraphs[idx].text, " ");
                strcat(paragraphs[idx].text, text);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            scanf("%s", id);
            fgets(text, MAX_TEXT, stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            
            int idx = find_paragraph(id);
            if (idx != -1) {
                strcpy(paragraphs[idx].text, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            int idx = find_paragraph(id);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) {
                    paragraphs[i] = paragraphs[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", id);
            int idx = find_paragraph(id);
            if (idx != -1) {
                print_with_underscores(paragraphs[idx].text);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", paragraphs[i].id);
            }
        }
    }
    
    return 0;
}