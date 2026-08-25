// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *stop; char *note; } Stop;
Stop *stops = NULL; int ns = 0;

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
            stops=realloc(stops,sizeof(Stop)*(ns+1));
            stops[ns].stop=strdup(arg1); stops[ns].note=strdup(p); ns++;
        } else if(!strcmp(cmd,"INSERT")) {
            int idx; sscanf(line,"%*s %d %s",&idx,arg1);
            if(idx<0 || idx>ns) continue;
            char *p=line; while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            for(int i=ns;i>idx;i--) stops[i]=stops[i-1];
            stops[idx].stop=strdup(arg1); stops[idx].note=strdup(p); ns++;
        } else if(!strcmp(cmd,"MOVE")) {
            int f,t; sscanf(line,"%*s %d %d",&f,&t);
            if(f<0||f>=ns||t<0||t>=ns) continue;
            Stop tmp=stops[f];
            if(f<t) for(int i=f;i<t;i++) stops[i]=stops[i+1];
            else for(int i=f;i>t;i--) stops[i]=stops[i-1];
            stops[t]=tmp;
        } else if(!strcmp(cmd,"REMOVE")) {
            int idx; sscanf(line,"%*s %d",&idx);
            if(idx<0||idx>=ns) continue;
            free(stops[idx].stop); free(stops[idx].note);
            for(int i=idx;i<ns-1;i++) stops[i]=stops[i+1];
            ns--;
        } else if(!strcmp(cmd,"FIND")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<ns;i++) if(!strcmp(stops[i].stop,arg1)) printf("%s\n",stops[i].note);
        } else if(!strcmp(cmd,"PRINT")) {
            for(int i=0;i<ns;i++) printf("%s %s\n",stops[i].stop,stops[i].note);
        }
    }
    return 0;
}