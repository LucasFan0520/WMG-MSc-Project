// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note *notes = NULL;
int ncount = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char cat[100], text[1000];
            scanf("%s", cat);
            scanf(" %[^\n]", text);
            notes = realloc(notes, sizeof(Note) * (ncount + 1));
            notes[ncount].category = strdup(cat);
            notes[ncount].text = strdup(text);
            ncount++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[100];
            scanf("%s", cat);
            for (int i = 0; i < ncount; ) {
                if (strcmp(notes[i].category, cat) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < ncount - 1; j++) notes[j] = notes[j+1];
                    ncount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[100];
            scanf("%s", cat);
            for (int i = 0; i < ncount; ) {
                if (strcmp(notes[i].category, cat) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < ncount - 1; j++) notes[j] = notes[j+1];
                    ncount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[100];
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
    for (int i = 0; i < ncount; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}