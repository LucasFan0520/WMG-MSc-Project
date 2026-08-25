// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Note;
Note *notes = 0; int n = 0, cap = 0;
char filepath[1024] = {0};

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(notes[i].name, name)) return i;
    return -1;
}

void load() {
    if (!filepath[0]) return;
    FILE *f = fopen(filepath, "r");
    if (!f) return;
    Note *tmp = 0; int tn = 0, tcap = 0;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char nm[256], nt[2048]; nm[0] = nt[0] = 0;
        if (sscanf(line, "%s %[^\n]", nm, nt) < 2) { valid = 0; break; }
        if (tn == tcap) { tcap = tcap ? tcap * 2 : 4; tmp = realloc(tmp, tcap * sizeof(Note)); }
        tmp[tn].name = strdup(nm); tmp[tn].note = strdup(nt); tn++;
    }
    fclose(f);
    if (valid) {
        for (int i = 0; i < n; i++) { free(notes[i].name); free(notes[i].note); }
        free(notes); notes = tmp; n = tn; cap = tcap;
    } else {
        for (int i = 0; i < tn; i++) { free(tmp[i].name); free(tmp[i].note); }
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) strcpy(filepath, argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[2048];
        a[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else if (!strcmp(cmd, "DELETE")) sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "LOAD")) load();
        else if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap * 2 : 4; notes = realloc(notes, cap * sizeof(Note)); }
            notes[n].name = strdup(a); notes[n].note = strdup(rest); n++;
        } else if (!strcmp(cmd, "DELETE")) {
            int idx = find(a);
            if (idx >= 0) {
                free(notes[idx].name); free(notes[idx].note);
                for (int i = idx; i < n - 1; i++) notes[i] = notes[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < n; i++) printf("%s %s\n", notes[i].name, notes[i].note);
        }
    }
    return 0;
}