// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Note;
Note *notes; int n_note;

void load(char *fname) {
    FILE *f = fopen(fname, "r");
    if(!f) return;
    Note *tmp = NULL; int n_tmp = 0;
    char line[1000];
    while(fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        char *sp = strchr(line, ' ');
        if(sp) {
            *sp = 0;
            tmp = realloc(tmp, (n_tmp+1)*sizeof(Note));
            tmp[n_tmp].name = strdup(line);
            tmp[n_tmp].note = strdup(sp+1);
            n_tmp++;
        }
    }
    fclose(f);
    for(int i=0; i<n_note; i++) { free(notes[i].name); free(notes[i].note); }
    free(notes);
    notes = tmp;
    n_note = n_tmp;
}

int main(int argc, char **argv) {
    char cmd[20], name[100], note[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "LOAD")) {
            if(argc > 1) load(argv[1]);
        } else if(!strcmp(cmd, "ADD")) {
            scanf("%s", name);
            fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            notes = realloc(notes, (n_note+1)*sizeof(Note));
            notes[n_note].name = strdup(name);
            notes[n_note].note = strdup(note);
            n_note++;
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            for(int i=0; i<n_note; ) {
                if(!strcmp(notes[i].name, name)) {
                    free(notes[i].name); free(notes[i].note);
                    for(int k=i; k<n_note-1; k++) notes[k] = notes[k+1];
                    n_note--;
                } else i++;
            }
        } else if(!strcmp(cmd, "REPORT")) {
            for(int i=0; i<n_note; i++) printf("%s %s\n", notes[i].name, notes[i].note);
        }
    }
    return 0;
}