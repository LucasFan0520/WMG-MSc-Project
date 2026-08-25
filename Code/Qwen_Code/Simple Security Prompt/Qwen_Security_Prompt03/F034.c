// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Note;
Note *notes = NULL;
int nn = 0;

void clear_notes() {
    for (int i = 0; i < nn; i++) { free(notes[i].name); free(notes[i].note); }
    free(notes);
    notes = NULL;
    nn = 0;
}

int main(int argc, char **argv) {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                FILE *f = fopen(argv[1], "r");
                if (f) {
                    Note *temp = NULL;
                    int tn = 0;
                    int valid = 1;
                    char fline[2048];
                    while (fgets(fline, sizeof(fline), f)) {
                        fline[strcspn(fline, "\n")] = 0;
                        char *space = strchr(fline, ' ');
                        if (!space) { valid = 0; break; }
                        *space = 0;
                        temp = realloc(temp, (tn + 1) * sizeof(Note));
                        temp[tn].name = strdup(fline);
                        temp[tn].note = strdup(space + 1);
                        tn++;
                    }
                    fclose(f);
                    if (valid) {
                        clear_notes();
                        notes = temp;
                        nn = tn;
                    } else {
                        for (int i = 0; i < tn; i++) { free(temp[i].name); free(temp[i].note); }
                        free(temp);
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            notes = realloc(notes, (nn + 1) * sizeof(Note));
            notes[nn].name = strdup(a);
            notes[nn].note = strdup(t);
            nn++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nn; i++) {
                if (strcmp(notes[i].name, a) == 0) {
                    free(notes[i].name); free(notes[i].note);
                    for (int j = i; j < nn - 1; j++) notes[j] = notes[j + 1];
                    nn--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nn; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    clear_notes();
    return 0;
}