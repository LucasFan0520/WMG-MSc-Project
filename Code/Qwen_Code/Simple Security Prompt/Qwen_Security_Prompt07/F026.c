// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *cat, *text; } Note;
Note *notes = 0; int n = 0, cap = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[2048];
        a[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap * 2 : 4; notes = realloc(notes, cap * sizeof(Note)); }
            notes[n].cat = strdup(a); notes[n].text = strdup(rest); n++;
        } else if (!strcmp(cmd, "DELETE")) {
            for (int i = 0; i < n; ) {
                if (!strcmp(notes[i].cat, a)) {
                    free(notes[i].cat); free(notes[i].text);
                    for (int j = i; j < n - 1; j++) notes[j] = notes[j + 1]; n--;
                } else i++;
            }
        } else if (!strcmp(cmd, "KEEP")) {
            for (int i = 0; i < n; ) {
                if (strcmp(notes[i].cat, a)) {
                    free(notes[i].cat); free(notes[i].text);
                    for (int j = i; j < n - 1; j++) notes[j] = notes[j + 1]; n--;
                } else i++;
            }
        } else if (!strcmp(cmd, "FIND")) {
            for (int i = 0; i < n; i++) {
                if (strstr(notes[i].text, a)) puts(notes[i].text);
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < n; i++) printf("%s %s\n", notes[i].cat, notes[i].text);
        }
    }
    return 0;
}