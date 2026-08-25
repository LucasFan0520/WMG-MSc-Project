// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

Note *notes = NULL;
int num_notes = 0;

int find_note(const char *name) {
    for (int i = 0; i < num_notes; i++) {
        if (strcmp(notes[i].name, name) == 0) return i;
    }
    return -1;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    
    Note *temp = NULL;
    int n_temp = 0;
    char line[4096];
    int valid = 1;
    
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256];
        if (sscanf(line, "%s", name) != 1) {
            valid = 0;
            break;
        }
        char *note = strchr(line, ' ');
        if (note) {
            note++;
        } else {
            note = "";
        }
        temp = realloc(temp, (n_temp + 1) * sizeof(Note));
        temp[n_temp].name = strdup(name);
        temp[n_temp].note = strdup(note);
        n_temp++;
    }
    fclose(f);
    
    if (valid) {
        for (int i = 0; i < num_notes; i++) {
            free(notes[i].name);
            free(notes[i].note);
        }
        free(notes);
        notes = temp;
        num_notes = n_temp;
    } else {
        for (int i = 0; i < n_temp; i++) {
            free(temp[i].name);
            free(temp[i].note);
        }
        free(temp);
    }
}

int main(int argc, char *argv[]) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                load_file(argv[1]);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            char *note = strstr(line, name);
            if (note) {
                note += strlen(name);
                if (*note == ' ') note++;
            } else {
                note = "";
            }
            int idx = find_note(name);
            if (idx >= 0) {
                free(notes[idx].note);
                notes[idx].note = strdup(note);
            } else {
                notes = realloc(notes, (num_notes + 1) * sizeof(Note));
                notes[num_notes].name = strdup(name);
                notes[num_notes].note = strdup(note);
                num_notes++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_note(name);
            if (idx >= 0) {
                free(notes[idx].name);
                free(notes[idx].note);
                for (int i = idx; i < num_notes - 1; i++) {
                    notes[i] = notes[i + 1];
                }
                num_notes--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < num_notes; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    
    for (int i = 0; i < num_notes; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    free(notes);
    
    return 0;
}