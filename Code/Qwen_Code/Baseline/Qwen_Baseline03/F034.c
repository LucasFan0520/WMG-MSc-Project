// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    char note[1000];
} Note;

Note notes[1000];
int nn = 0;

int findNote(char *name) {
    for (int i = 0; i < nn; i++) {
        if (!strcmp(notes[i].name, name)) return i;
    }
    return -1;
}

void load(char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    Note tmp[1000];
    int tn = 0;
    char line[1100];
    while (fgets(line, 1100, f)) {
        line[strcspn(line, "\n")] = 0;
        char *space = strchr(line, ' ');
        if (!space) {
            fclose(f);
            return;
        }
        *space = 0;
        if (strlen(line) >= 100) {
            fclose(f);
            return;
        }
        strcpy(tmp[tn].name, line);
        strcpy(tmp[tn].note, space + 1);
        tn++;
    }
    fclose(f);
    nn = tn;
    for (int i = 0; i < tn; i++) {
        notes[i] = tmp[i];
    }
}

int main(int argc, char **argv) {
    char cmd[20], name[100], note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "LOAD")) {
            if (argc > 1) load(argv[1]);
        } else if (!strcmp(cmd, "ADD")) {
            scanf("%s", name);
            fgets(note, 1000, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            strcpy(notes[nn].name, name);
            strcpy(notes[nn].note, note);
            nn++;
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int i = findNote(name);
            if (i != -1) {
                for (int j = i; j < nn - 1; j++) {
                    notes[j] = notes[j + 1];
                }
                nn--;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nn; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    return 0;
}