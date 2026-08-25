// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CAT 100
#define MAX_TEXT 1000

typedef struct {
    char category[MAX_CAT];
    char text[MAX_TEXT];
} Note;

Note notes[10000];
int note_count = 0;

int main() {
    char cmd[20], category[MAX_CAT], text[MAX_TEXT], word[MAX_TEXT];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", category);
            fgets(text, MAX_TEXT, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (text[strlen(text) - 1] == '\n') text[strlen(text) - 1] = '\0';
            strcpy(notes[note_count].category, category);
            strcpy(notes[note_count].text, text);
            note_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", category);
            int i = 0;
            while (i < note_count) {
                if (strcmp(notes[i].category, category) == 0) {
                    for (int j = i; j < note_count - 1; j++) notes[j] = notes[j + 1];
                    note_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            scanf("%s", category);
            int i = 0;
            while (i < note_count) {
                if (strcmp(notes[i].category, category) != 0) {
                    for (int j = i; j < note_count - 1; j++) notes[j] = notes[j + 1];
                    note_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", word);
            for (int i = 0; i < note_count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < note_count; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    return 0;
}