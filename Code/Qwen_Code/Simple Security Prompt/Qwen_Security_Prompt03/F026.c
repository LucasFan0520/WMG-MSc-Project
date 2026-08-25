// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *category; char *text; } Note;
Note *notes = NULL;
int nn = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            notes = realloc(notes, (nn + 1) * sizeof(Note));
            notes[nn].category = strdup(a);
            notes[nn].text = strdup(t);
            nn++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nn; ) {
                if (strcmp(notes[i].category, a) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < nn - 1; j++) notes[j] = notes[j + 1];
                    nn--;
                } else i++;
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nn; ) {
                if (strcmp(notes[i].category, a) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < nn - 1; j++) notes[j] = notes[j + 1];
                    nn--;
                } else i++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nn; i++) {
                if (strstr(notes[i].text, a)) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < nn; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    for (int i = 0; i < nn; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}