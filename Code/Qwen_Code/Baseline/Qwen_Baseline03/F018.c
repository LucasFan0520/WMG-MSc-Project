// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char word[100]; int count; } W;
W arr[1000]; int n=0;
int find(char *w) { for(int i=0;i<n;i++) if(!strcmp(arr[i].word,w)) return i; return -1; }
int cmp(const void *a, const void *b) { return ((W*)b)->count - ((W*)a)->count; }

void load(char *fname) {
    FILE *f=fopen(fname,"r"); if(!f) return;
    char line[100];
    while(fgets(line,100,f)) {
        line[strcspn(line,"\n")]=0;
        if(line[0]) { strcpy(arr[n].word,line); arr[n].count=1; n++; }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char cmd[20], word[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"WORD")) {
            fgets(word,100,stdin); word[strcspn(word,"\n")]=0;
            if(word[0]==' ') memmove(word,word+1,strlen(word));
            int i=find(word);
            if(i!=-1) arr[i].count++;
            else { strcpy(arr[n].word,word); arr[n].count=1; n++; }
        } else if(!strcmp(cmd,"REMOVE")) {
            fgets(word,100,stdin); word[strcspn(word,"\n")]=0;
            if(word[0]==' ') memmove(word,word+1,strlen(word));
            int i=find(word);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"COUNT")) {
            fgets(word,100,stdin); word[strcspn(word,"\n")]=0;
            if(word[0]==' ') memmove(word,word+1,strlen(word));
            int i=find(word); printf("%d\n", i!=-1 ? arr[i].count : 0);
        } else if(!strcmp(cmd,"TOP")) {
            qsort(arr,n,sizeof(W),cmp);
            for(int i=0;i<n;i++) printf("%s %d\n",arr[i].word,arr[i].count);
        }
    }
    return 0;
}