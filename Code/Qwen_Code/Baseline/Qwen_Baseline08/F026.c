// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOTES 10000
#define MAX_LEN 1000

typedef struct {
    char category[100];
    char text[MAX_LEN];
} Note;

Note notes[MAX_NOTES];
int ncount = 0;

int main() {
    char cmd[20], cat[100], text[MAX_LEN], word[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", cat);
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (ncount < MAX_NOTES) {
                strcpy(notes[ncount].category, cat);
                strcpy(notes[ncount].text, text);
                ncount++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", cat);
            for (int i = 0; i < ncount; ) {
                if (strcmp(notes[i].category, cat) == 0) {
                    for (int j = i; j < ncount - 1; j++) notes[j] = notes[j + 1];
                    ncount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            scanf("%s", cat);
            for (int i = 0; i < ncount; ) {
                if (strcmp(notes[i].category, cat) != 0) {
                    for (int j = i; j < ncount - 1; j++) notes[j] = notes[j + 1];
                    ncount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", word);
            for (int i = 0; i < ncount; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < ncount; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    return 0;
}