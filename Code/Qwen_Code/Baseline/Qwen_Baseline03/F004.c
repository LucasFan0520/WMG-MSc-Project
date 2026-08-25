// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char key[100]; char val[1000]; } V;
V arr[1000]; int n=0;
int find(char *k) { for(int i=0;i<n;i++) if(!strcmp(arr[i].key,k)) return i; return -1; }

int main() {
    char cmd[20], k[100], v[1000], text[2000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"SET")) {
            scanf("%s", k); fgets(v,1000,stdin); v[strcspn(v,"\n")]=0;
            if(v[0]==' ') memmove(v,v+1,strlen(v));
            int i=find(k);
            if(i!=-1) strcpy(arr[i].val,v);
            else { strcpy(arr[n].key,k); strcpy(arr[n].val,v); n++; }
        } else if(!strcmp(cmd,"UNSET")) {
            scanf("%s", k); int i=find(k);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"RENDER")) {
            fgets(text,2000,stdin); text[strcspn(text,"\n")]=0;
            if(text[0]==' ') memmove(text,text+1,strlen(text));
            char out[4000]=""; int p=0;
            for(int i=0;text[i];) {
                if(text[i]=='{' && strchr(text+i,'}')) {
                    char tk[100]; int j=0; i++;
                    while(text[i]!='}') tk[j++]=text[i++];
                    tk[j]=0; i++;
                    int idx=find(tk);
                    if(idx!=-1) { strcpy(out+p,arr[idx].val); p+=strlen(arr[idx].val); }
                    else { out[p++]='{'; strcpy(out+p,tk); p+=j; out[p++]='}'; }
                } else out[p++]=text[i++];
            }
            out[p]=0; printf("%s\n",out);
        } else if(!strcmp(cmd,"VARS")) {
            for(int i=0;i<n;i++) printf("%s %s\n",arr[i].key,arr[i].val);
        }
    }
    return 0;
}