// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *sender; char *subject; char *body; char **tags; int nt; } Mail;
Mail *mails = NULL; int nm = 0;

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000], arg2[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"MAIL")) {
            char *p=line+5; while(*p==' ')p++;
            char sender[1000]; sscanf(p,"%s",sender);
            p+=strlen(sender); while(*p==' ')p++;
            char subject[1000]; sscanf(p,"%s",subject);
            p+=strlen(subject); while(*p==' ')p++;
            mails=realloc(mails,sizeof(Mail)*(nm+1));
            mails[nm].sender=strdup(sender);
            mails[nm].subject=strdup(subject);
            mails[nm].body=strdup(p);
            mails[nm].tags=NULL; mails[nm].nt=0;
            nm++;
        } else if(!strcmp(cmd,"TAG")) {
            sscanf(line,"%*s %s %s",arg1,arg2);
            for(int i=0;i<nm;i++) {
                if(!strcmp(mails[i].subject,arg1)) {
                    mails[i].tags=realloc(mails[i].tags,sizeof(char*)*(mails[i].nt+1));
                    mails[i].tags[mails[i].nt++]=strdup(arg2);
                    break;
                }
            }
        } else if(!strcmp(cmd,"UNTAG")) {
            sscanf(line,"%*s %s %s",arg1,arg2);
            for(int i=0;i<nm;i++) {
                if(!strcmp(mails[i].subject,arg1)) {
                    for(int j=0;j<mails[i].nt;j++) {
                        if(!strcmp(mails[i].tags[j],arg2)) {
                            free(mails[i].tags[j]);
                            for(int k=j;k<mails[i].nt-1;k++) mails[i].tags[k]=mails[i].tags[k+1];
                            mails[i].nt--; break;
                        }
                    }
                    break;
                }
            }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nm;i++) {
                if(!strcmp(mails[i].subject,arg1)) {
                    free(mails[i].sender); free(mails[i].subject); free(mails[i].body);
                    for(int j=0;j<mails[i].nt;j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    for(int j=i;j<nm-1;j++) mails[j]=mails[j+1];
                    nm--; break;
                }
            }
        } else if(!strcmp(cmd,"SEARCH")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nm;i++) {
                for(int j=0;j<mails[i].nt;j++) {
                    if(!strcmp(mails[i].tags[j],arg1)) {
                        printf("%s\n",mails[i].subject); break;
                    }
                }
            }
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<nm;i++) {
                printf("%s %s %s",mails[i].sender,mails[i].subject,mails[i].body);
                for(int j=0;j<mails[i].nt;j++) printf(" %s",mails[i].tags[j]);
                printf("\n");
            }
        }
    }
    return 0;
}