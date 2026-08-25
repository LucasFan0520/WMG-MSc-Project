// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *sender, *subject, *body, **tags; int n_tag; } Mail;
Mail *mails; int n_mail;

int main() {
    char cmd[20], sender[100], subject[100], body[1000], tag[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "MAIL")) {
            scanf("%s %s", sender, subject);
            fgets(body, 1000, stdin);
            while(body[0]==' ') memmove(body, body+1, strlen(body));
            body[strcspn(body, "\n")] = 0;
            mails = realloc(mails, (n_mail+1)*sizeof(Mail));
            mails[n_mail].sender = strdup(sender);
            mails[n_mail].subject = strdup(subject);
            mails[n_mail].body = strdup(body);
            mails[n_mail].tags = NULL;
            mails[n_mail].n_tag = 0;
            n_mail++;
        } else if(!strcmp(cmd, "TAG")) {
            scanf("%s %s", subject, tag);
            for(int i=0; i<n_mail; i++) {
                if(!strcmp(mails[i].subject, subject)) {
                    mails[i].tags = realloc(mails[i].tags, (mails[i].n_tag+1)*sizeof(char*));
                    mails[i].tags[mails[i].n_tag++] = strdup(tag);
                    break;
                }
            }
        } else if(!strcmp(cmd, "UNTAG")) {
            scanf("%s %s", subject, tag);
            for(int i=0; i<n_mail; i++) {
                if(!strcmp(mails[i].subject, subject)) {
                    for(int j=0; j<mails[i].n_tag; j++) {
                        if(!strcmp(mails[i].tags[j], tag)) {
                            free(mails[i].tags[j]);
                            for(int k=j; k<mails[i].n_tag-1; k++) mails[i].tags[k] = mails[i].tags[k+1];
                            mails[i].n_tag--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", subject);
            for(int i=0; i<n_mail; i++) {
                if(!strcmp(mails[i].subject, subject)) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for(int j=0; j<mails[i].n_tag; j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    for(int k=i; k<n_mail-1; k++) mails[k] = mails[k+1];
                    n_mail--;
                    break;
                }
            }
        } else if(!strcmp(cmd, "SEARCH")) {
            scanf("%s", tag);
            for(int i=0; i<n_mail; i++) {
                for(int j=0; j<mails[i].n_tag; j++) {
                    if(!strcmp(mails[i].tags[j], tag)) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if(!strcmp(cmd, "REPORT")) {
            for(int i=0; i<n_mail; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for(int j=0; j<mails[i].n_tag; j++) printf(" %s", mails[i].tags[j]);
                printf("\n");
            }
        }
    }
    return 0;
}