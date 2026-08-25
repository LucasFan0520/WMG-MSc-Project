// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteRec;

NoteRec *notes = NULL;
int nc = 0;

int find_note(const char *name) {
    for (int i = 0; i < nc; i++) {
        if (strcmp(notes[i].name, name) == 0) return i;
    }
    return -1;
}

void free_all_notes(void) {
    for (int i = 0; i < nc; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    free(notes);
    notes = NULL;
    nc = 0;
}

int load_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    NoteRec *tmp = NULL;
    int tc = 0;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char nm[256];
        char nt[2048];
        nm[0] = '\0';
        nt[0] = '\0';
        int pn = sscanf(line, "%255s %2047[^\n]", nm, nt);
        if (pn < 2) {
            for (int i = 0; i < tc; i++) {
                free(tmp[i].name);
                free(tmp[i].note);
            }
            free(tmp);
            fclose(f);
            return -1;
        }
        tmp = realloc(tmp, (tc + 1) * sizeof(NoteRec));
        if (!tmp) {
            fclose(f);
            return -1;
        }
        tmp[tc].name = strdup(nm);
        tmp[tc].note = strdup(nt);
        tc++;
    }
    fclose(f);
    free_all_notes();
    notes = tmp;
    nc = tc;
    return 0;
}

int main(int argc, char *argv[]) {
    char filepath[1024];
    filepath[0] = '\0';
    if (argc > 1) {
        strncpy(filepath, argv[1], sizeof(filepath) - 1);
        filepath[sizeof(filepath) - 1] = '\0';
    }
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

        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath[0] != '\0') {
                load_from_file(filepath);
            }
        } else if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            notes = realloc(notes, (nc + 1) * sizeof(NoteRec));
            if (!notes) return 1;
            notes[nc].name = strdup(a1);
            notes[nc].note = strdup(rest);
            nc++;
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int ni = find_note(a1);
            if (ni >= 0) {
                free(notes[ni].name);
                free(notes[ni].note);
                for (int k = ni; k < nc - 1; k++) {
                    notes[k] = notes[k + 1];
                }
                nc--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nc; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    free_all_notes();
    return 0;
}