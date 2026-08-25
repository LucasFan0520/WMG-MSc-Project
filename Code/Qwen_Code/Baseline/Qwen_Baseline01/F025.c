// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Stu;
typedef struct { char *name; Stu *stus; int n; } Class;
Class *classes = NULL; int nc = 0;

int find_class(const char *n) {
    for(int i=0;i<nc;i++) if(!strcmp(classes[i].name,n)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000], arg2[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"CLASS")) {
            sscanf(line,"%*s %s",arg1);
            classes=realloc(classes,sizeof(Class)*(nc+1));
            classes[nc].name=strdup(arg1); classes[nc].stus=NULL; classes[nc].n=0; nc++;
        } else if(!strcmp(cmd,"STUDENT")) {
            sscanf(line,"%*s %s %s",arg1,arg2);
            char *p=line+8+strlen(arg1)+1+strlen(arg2); while(*p==' ')p++;
            int i=find_class(arg1);
            if(i!=-1) {
                classes[i].stus=realloc(classes[i].stus,sizeof(Stu)*(classes[i].n+1));
                classes[i].stus[classes[i].n].name=strdup(arg2);
                classes[i].stus[classes[i].n].note=strdup(p);
                classes[i].n++;
            }
        } else if(!strcmp(cmd,"TRANSFER")) {
            char from[1000], to[1000], stu[1000];
            sscanf(line,"%*s %s %s %s",from,to,stu);
            int i1=find_class(from), i2=find_class(to);
            if(i1!=-1 && i2!=-1) {
                for(int j=0;j<classes[i1].n;j++) {
                    if(!strcmp(classes[i1].stus[j].name,stu)) {
                        classes[i2].stus=realloc(classes[i2].stus,sizeof(Stu)*(classes[i2].n+1));
                        classes[i2].stus[classes[i2].n]=classes[i1].stus[j];
                        classes[i2].n++;
                        for(int k=j;k<classes[i1].n-1;k++) classes[i1].stus[k]=classes[i1].stus[k+1];
                        classes[i1].n--; break;
                    }
                }
            }
        } else if(!strcmp(cmd,"DROP")) {
            char cls[1000], stu[1000];
            sscanf(line,"%*s %s %s",cls,stu);
            int i=find_class(cls);
            if(i!=-1) {
                for(int j=0;j<classes[i].n;j++) {
                    if(!strcmp(classes[i].stus[j].name,stu)) {
                        free(classes[i].stus[j].name); free(classes[i].stus[j].note);
                        for(int k=j;k<classes[i].n-1;k++) classes[i].stus[k]=classes[i].stus[k+1];
                        classes[i].n--; break;
                    }
                }
            }
        } else if(!strcmp(cmd,"DELETECLASS")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_class(arg1);
            if(i!=-1) {
                free(classes[i].name);
                for(int j=0;j<classes[i].n;j++) { free(classes[i].stus[j].name); free(classes[i].stus[j].note); }
                free(classes[i].stus);
                for(int j=i;j<nc-1;j++) classes[j]=classes[j+1];
                nc--;
            }
        } else if(!strcmp(cmd,"PRINT")) {
            for(int i=0;i<nc;i++) {
                printf("%s\n",classes[i].name);
                for(int j=0;j<classes[i].n;j++) printf(" %s %s\n",classes[i].stus[j].name,classes[i].stus[j].note);
            }
        }
    }
    return 0;
}