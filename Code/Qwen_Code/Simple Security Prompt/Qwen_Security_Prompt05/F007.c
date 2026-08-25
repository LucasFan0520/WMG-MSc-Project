// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int time; char *label, *note; } A;
A *as = NULL; int na = 0;

int cmp(const void *a, const void *b) { A *x=(A*)a, *y=(A*)b; if(x->time!=y->time) return x->time-y->time; return strcmp(x->label,y->label); }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], label[256], note[1024]=""; int t;
        sscanf(ln,"%s %d %s",cmd,&t,label);
        char *p=strstr(ln,label); if(p) { p+=strlen(label); if(*p) p++; strcpy(note,p); }
        if(!strcmp(cmd,"ADD")) { as=realloc(as,(na+1)*sizeof(A)); as[na].time=t; as[na].label=strdup(label); as[na].note=strdup(note); na++; }
        else if(!strcmp(cmd,"DELETE")) { for(int i=0;i<na;) { if(as[i].time==t && !strcmp(as[i].label,label)) { free(as[i].label); free(as[i].note); for(int j=i;j<na-1;j++) as[j]=as[j+1]; na--; } else i++; } }
        else if(!strcmp(cmd,"FIND")) { int f=-1; for(int i=0;i<na;i++) if(!strcmp(as[i].label,label) && (f==-1 || as[i].time<as[f].time)) f=i; if(f>=0) printf("%d %s %s\n",as[f].time,as[f].label,as[f].note); }
        else if(!strcmp(cmd,"SORT")) { qsort(as,na,sizeof(A),cmp); for(int i=0;i<na;i++) printf("%d %s %s\n",as[i].time,as[i].label,as[i].note); }
        else if(!strcmp(cmd,"CLEAR")) { for(int i=0;i<na;i++) { free(as[i].label); free(as[i].note); } na=0; }
    }
    for(int i=0;i<na;i++) { free(as[i].label); free(as[i].note); } free(as);
    return 0;
}