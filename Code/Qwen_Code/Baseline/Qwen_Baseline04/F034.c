// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

Note *notes = NULL;
int ncount = 0;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *temp = NULL;
    int tcount = 0;
    char line[2000];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *space = strchr(line, ' ');
        if (!space) { valid = 0; break; }
        *space = '\0';
        temp = realloc(temp, sizeof(Note) * (tcount + 1));
        temp[tcount].name = strdup(line);
        temp[tcount].note = strdup(space + 1);
        tcount++;
    }
    fclose(f);
    if (valid) {
        for (int i = 0; i < ncount; i++) {
            free(notes[i].name);
            free(notes[i].note);
        }
        free(notes);
        notes = temp;
        ncount = tcount;
    } else {
        for (int i = 0; i < tcount; i++) {
            free(temp[i].name);
            free(temp[i].note);
        }
        free(temp);
    }
}

int main(int argc, char *argv[]) {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[100], note[1000];
            scanf("%s", name);
            scanf(" %[^\n]", note);
            notes = realloc(notes, sizeof(Note) * (ncount + 1));
            notes[ncount].name = strdup(name);
            notes[ncount].note = strdup(note);
            ncount++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            for (int i = 0; i < ncount; i++) {
                if (strcmp(notes[i].name, name) == 0) {
                    free(notes[i].name);
                    free(notes[i].note);
                    for (int j = i; j < ncount - 1; j++) notes[j] = notes[j+1];
                    ncount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ncount; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    for (int i = 0; i < ncount; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    free(notes);
    return 0;
}