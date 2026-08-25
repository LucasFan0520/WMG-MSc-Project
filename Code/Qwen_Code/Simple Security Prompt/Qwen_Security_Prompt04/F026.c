// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note notes[10000];
int nc = 0;

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char cat[1000];
            sscanf(rest, "%s", cat);
            char *text = rest + strlen(cat);
            while (*text == ' ') text++;
            notes[nc].category = strdup(cat);
            notes[nc].text = strdup(text);
            nc++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < nc; i++) {
                if (strcmp(notes[i].category, rest) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < nc - 1; j++) notes[j] = notes[j+1];
                    nc--;
                    i--;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            for (int i = 0; i < nc; i++) {
                if (strcmp(notes[i].category, rest) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < nc - 1; j++) notes[j] = notes[j+1];
                    nc--;
                    i--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < nc; i++) {
                if (strstr(notes[i].text, rest) != NULL) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < nc; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    for (int i = 0; i < nc; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    return 0;
}