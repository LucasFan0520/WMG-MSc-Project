// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Note;
Note notes[10000]; int nnotes = 0;

int find_note(char *name) {
    for (int i = 0; i < nnotes; i++) if (!strcmp(notes[i].name, name)) return i;
    return -1;
}

void load(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *temp = malloc(sizeof(Note) * 10000);
    int ntemp = 0;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *space = strchr(line, ' ');
        if (space) {
            *space = 0;
            char *n = space + 1;
            while(*n == ' ') n++;
            temp[ntemp].name = strdup(line);
            temp[ntemp++].note = strdup(n);
        } else {
            for (int i = 0; i < ntemp; i++) { free(temp[i].name); free(temp[i].note); }
            free(temp);
            fclose(f);
            return;
        }
    }
    fclose(f);
    for (int i = 0; i < nnotes; i++) { free(notes[i].name); free(notes[i].note); }
    nnotes = ntemp;
    memcpy(notes, temp, sizeof(Note) * nnotes);
    free(temp);
}

int main(int argc, char **argv) {
    char cmd[20], name[100], note[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "LOAD")) {
            if (argc > 1) load(argv[1]);
        } else if (!strcmp(cmd, "ADD")) {
            scanf("%s", name);
            fgets(note, sizeof(note), stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int idx = find_note(name);
            if (idx != -1) { free(notes[idx].note); notes[idx].note = strdup(note); }
            else { notes[nnotes].name = strdup(name); notes[nnotes++].note = strdup(note); }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_note(name);
            if (idx != -1) {
                free(notes[idx].name); free(notes[idx].note);
                for (int i = idx; i < nnotes - 1; i++) notes[i] = notes[i + 1];
                nnotes--;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nnotes; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    for (int i = 0; i < nnotes; i++) { free(notes[i].name); free(notes[i].note); }
    return 0;
}