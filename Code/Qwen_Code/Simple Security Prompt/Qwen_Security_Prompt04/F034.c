// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

Note notes[10000];
int nc = 0;

int find_name(char *name) {
    for (int i = 0; i < nc; i++) if (strcmp(notes[i].name, name) == 0) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note temp[10000];
    int tc = 0;
    char line[10000];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[1000];
        sscanf(line, "%s", name);
        char *note = line + strlen(name);
        while (*note == ' ') note++;
        if (strlen(name) == 0) {
            valid = 0;
            break;
        }
        temp[tc].name = strdup(name);
        temp[tc].note = strdup(note);
        tc++;
    }
    fclose(f);
    if (valid) {
        for (int i = 0; i < nc; i++) {
            free(notes[i].name);
            free(notes[i].note);
        }
        nc = tc;
        for (int i = 0; i < nc; i++) {
            notes[i] = temp[i];
        }
    } else {
        for (int i = 0; i < tc; i++) {
            free(temp[i].name);
            free(temp[i].note);
        }
    }
}

int main(int argc, char *argv[]) {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *note = rest + strlen(name);
            while (*note == ' ') note++;
            if (find_name(name) == -1) {
                notes[nc].name = strdup(name);
                notes[nc].note = strdup(note);
                nc++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_name(rest);
            if (idx != -1) {
                free(notes[idx].name);
                free(notes[idx].note);
                for (int i = idx; i < nc - 1; i++) notes[i] = notes[i+1];
                nc--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nc; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    for (int i = 0; i < nc; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    return 0;
}