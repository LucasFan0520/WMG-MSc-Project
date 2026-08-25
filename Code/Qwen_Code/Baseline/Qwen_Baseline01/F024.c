// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char **authors; int na; int year; char *title; } Book;
Book *books = NULL; int nb = 0;

int find_book(const char *t) {
    for(int i=0;i<nb;i++) if(!strcmp(books[i].title,t)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"BOOK")) {
            int y; sscanf(line,"%*s %s %d",arg1,&y);
            books=realloc(books,sizeof(Book)*(nb+1));
            books[nb].title=strdup(arg1);
            books[nb].year=y;
            books[nb].authors=NULL; books[nb].na=0; nb++;
        } else if(!strcmp(cmd,"AUTHOR")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+7+strlen(arg1); while(*p==' ')p++;
            int i=find_book(arg1);
            if(i!=-1) {
                books[i].authors=realloc(books[i].authors,sizeof(char*)*(books[i].na+1));
                books[i].authors[books[i].na++]=strdup(p);
            }
        } else if(!strcmp(cmd,"REMOVEAUTHOR")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+13+strlen(arg1); while(*p==' ')p++;
            int i=find_book(arg1);
            if(i!=-1) {
                for(int j=0;j<books[i].na;j++) {
                    if(!strcmp(books[i].authors[j],p)) {
                        free(books[i].authors[j]);
                        for(int k=j;k<books[i].na-1;k++) books[i].authors[k]=books[i].authors[k+1];
                        books[i].na--; break;
                    }
                }
            }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_book(arg1);
            if(i!=-1) {
                free(books[i].title);
                for(int j=0;j<books[i].na;j++) free(books[i].authors[j]);
                free(books[i].authors);
                for(int j=i;j<nb-1;j++) books[j]=books[j+1];
                nb--;
            }
        } else if(!strcmp(cmd,"FIND")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_book(arg1);
            if(i!=-1) {
                for(int j=0;j<books[i].na;j++) printf("%s\n",books[i].authors[j]);
            }
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<nb;i++) printf("%s %d\n",books[i].title,books[i].year);
        }
    }
    return 0;
}