// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note *notes = NULL;
int nc = 0;

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char rest[2048];
        a1[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %2047[^\n]", cmd, a1, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            notes = realloc(notes, (nc + 1) * sizeof(Note));
            if (!notes) return 1;
            notes[nc].category = strdup(a1);
            notes[nc].text = strdup(rest);
            nc++;
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            for (int i = 0; i < nc; ) {
                if (strcmp(notes[i].category, a1) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int k = i; k < nc - 1; k++) {
                        notes[k] = notes[k + 1];
                    }
                    nc--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0 && n >= 2) {
            for (int i = 0; i < nc; ) {
                if (strcmp(notes[i].category, a1) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int k = i; k < nc - 1; k++) {
                        notes[k] = notes[k + 1];
                    }
                    nc--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            for (int i = 0; i < nc; i++) {
                if (strstr(notes[i].text, a1) != NULL) {
                    printf("%s %s\n", notes[i].category, notes[i].text);
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
    free(notes);
    return 0;
}