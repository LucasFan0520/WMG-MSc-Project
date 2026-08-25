// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note *notes = NULL;
int num_notes = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ADD") == 0) {
            char cat[256];
            sscanf(line, "%*s %s", cat);
            char *text = strstr(line, cat);
            if (text) {
                text += strlen(cat);
                if (*text == ' ') text++;
            } else {
                text = "";
            }
            notes = realloc(notes, (num_notes + 1) * sizeof(Note));
            notes[num_notes].category = strdup(cat);
            notes[num_notes].text = strdup(text);
            num_notes++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[256];
            sscanf(line, "%*s %s", cat);
            for (int i = 0; i < num_notes; ) {
                if (strcmp(notes[i].category, cat) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < num_notes - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    num_notes--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[256];
            sscanf(line, "%*s %s", cat);
            for (int i = 0; i < num_notes; ) {
                if (strcmp(notes[i].category, cat) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < num_notes - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    num_notes--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256];
            sscanf(line, "%*s %s", word);
            for (int i = 0; i < num_notes; i++) {
                if (strstr(notes[i].text, word)) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < num_notes; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    
    for (int i = 0; i < num_notes; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    
    return 0;
}