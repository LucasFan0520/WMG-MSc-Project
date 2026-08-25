// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Note;
Note *notes = NULL; int nn = 0;

int find_note(const char *n) {
    for(int i=0;i<nn;i++) if(!strcmp(notes[i].name,n)) return i;
    return -1;
}

void load(const char *path) {
    FILE *f=fopen(path,"r"); if(!f) return;
    Note *tmp=NULL; int tn=0;
    char line[10000];
    while(fgets(line,sizeof(line),f)) {
        line[strcspn(line,"\n")]=0;
        char name[1000]; sscanf(line,"%s",name);
        char *note=line+strlen(name); while(*note==' ')note++;
        tmp=realloc(tmp,sizeof(Note)*(tn+1));
        tmp[tn].name=strdup(name); tmp[tn].note=strdup(note); tn++;
    }
    fclose(f);
    for(int i=0;i<nn;i++) { free(notes[i].name); free(notes[i].note); }
    free(notes);
    notes=tmp; nn=tn;
}

int main(int argc, char **argv) {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"LOAD")) {
            if(argc>1) load(argv[1]);
        } else if(!strcmp(cmd,"ADD")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+4+strlen(arg1); while(*p==' ')p++;
            int i=find_note(arg1);
            if(i!=-1) { free(notes[i].note); notes[i].note=strdup(p); }
            else { notes=realloc(notes,sizeof(Note)*(nn+1)); notes[nn].name=strdup(arg1); notes[nn].note=strdup(p); nn++; }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_note(arg1);
            if(i!=-1) {
                free(notes[i].name); free(notes[i].note);
                for(int j=i;j<nn-1;j++) notes[j]=notes[j+1];
                nn--;
            }
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<nn;i++) printf("%s %s\n",notes[i].name,notes[i].note);
        }
    }
    return 0;
}