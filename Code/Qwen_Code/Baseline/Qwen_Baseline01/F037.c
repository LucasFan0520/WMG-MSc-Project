// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; unsigned char *buf; int len; } Buf;
Buf *bufs = NULL; int nb = 0;

int find_buf(const char *n) {
    for(int i=0;i<nb;i++) if(!strcmp(bufs[i].name,n)) return i;
    return -1;
}

int hex2int(char c) {
    if(c>='0'&&c<='9') return c-'0';
    if(c>='a'&&c<='f') return c-'a'+10;
    if(c>='A'&&c<='F') return c-'A'+10;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"NEW") || !strcmp(cmd,"APPEND")) {
            sscanf(line,"%*s %s",arg1);
            char *p;
            if(!strcmp(cmd,"NEW")) { p=line+4+strlen(arg1); while(*p==' ')p++; }
            else { p=line+7+strlen(arg1); while(*p==' ')p++; }
            int i=find_buf(arg1);
            if(!strcmp(cmd,"NEW")) {
                if(i!=-1) { free(bufs[i].buf); bufs[i].len=0; }
                else { bufs=realloc(bufs,sizeof(Buf)*(nb+1)); i=nb++; bufs[i].name=strdup(arg1); bufs[i].buf=NULL; bufs[i].len=0; }
            }
            int sl=strlen(p);
            unsigned char *tmp=malloc(sl/2);
            int tl=0;
            for(int j=0;j+1<sl;j+=2) {
                int h=hex2int(p[j]), l=hex2int(p[j+1]);
                if(h==-1||l==-1) break;
                tmp[tl++]=(h<<4)|l;
            }
            bufs[i].buf=realloc(bufs[i].buf,bufs[i].len+tl);
            memcpy(bufs[i].buf+bufs[i].len,tmp,tl);
            bufs[i].len+=tl;
            free(tmp);
        } else if(!strcmp(cmd,"SLICE")) {
            int off,len; sscanf(line,"%*s %s %d %d",arg1,&off,&len);
            int i=find_buf(arg1);
            if(i!=-1) {
                for(int j=0;j<len && off+j<bufs[i].len;j++) printf("%02x",bufs[i].buf[off+j]);
                printf("\n");
            }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_buf(arg1);
            if(i!=-1) {
                free(bufs[i].name); free(bufs[i].buf);
                for(int j=i;j<nb-1;j++) bufs[j]=bufs[j+1];
                nb--;
            }
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<nb;i++) printf("%s %d\n",bufs[i].name,bufs[i].len);
        }
    }
    return 0;
}