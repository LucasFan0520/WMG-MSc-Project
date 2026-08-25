// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *cat; char *text; } Note;
Note *notes = NULL; int nn = 0;

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"ADD")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+4+strlen(arg1); while(*p==' ')p++;
            notes=realloc(notes,sizeof(Note)*(nn+1));
            notes[nn].cat=strdup(arg1); notes[nn].text=strdup(p); nn++;
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nn;) {
                if(!strcmp(notes[i].cat,arg1)) {
                    free(notes[i].cat); free(notes[i].text);
                    for(int j=i;j<nn-1;j++) notes[j]=notes[j+1];
                    nn--;
                } else i++;
            }
        } else if(!strcmp(cmd,"KEEP")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nn;) {
                if(!strcmp(notes[i].cat,arg1)) i++;
                else {
                    free(notes[i].cat); free(notes[i].text);
                    for(int j=i;j<nn-1;j++) notes[j]=notes[j+1];
                    nn--;
                }
            }
        } else if(!strcmp(cmd,"FIND")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nn;i++) {
                if(strstr(notes[i].text,arg1)) printf("%s\n",notes[i].text);
            }
        } else if(!strcmp(cmd,"PRINT")) {
            for(int i=0;i<nn;i++) printf("%s %s\n",notes[i].cat,notes[i].text);
        }
    }
    return 0;
}