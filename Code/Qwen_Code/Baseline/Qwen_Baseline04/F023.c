// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tcount;
} Mail;

Mail *mails = NULL;
int mcount = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[100], subject[100], body[1000];
            scanf("%s %s", sender, subject);
            scanf(" %[^\n]", body);
            mails = realloc(mails, sizeof(Mail) * (mcount + 1));
            mails[mcount].sender = strdup(sender);
            mails[mcount].subject = strdup(subject);
            mails[mcount].body = strdup(body);
            mails[mcount].tags = NULL;
            mails[mcount].tcount = 0;
            mcount++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[100], tag[100];
            scanf("%s %s", subject, tag);
            for (int i = 0; i < mcount; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    mails[i].tags = realloc(mails[i].tags, sizeof(char*) * (mails[i].tcount + 1));
                    mails[i].tags[mails[i].tcount++] = strdup(tag);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[100], tag[100];
            scanf("%s %s", subject, tag);
            for (int i = 0; i < mcount; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    for (int j = 0; j < mails[i].tcount; j++) {
                        if (strcmp(mails[i].tags[j], tag) == 0) {
                            free(mails[i].tags[j]);
                            for (int k = j; k < mails[i].tcount - 1; k++) {
                                mails[i].tags[k] = mails[i].tags[k+1];
                            }
                            mails[i].tcount--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[100];
            scanf("%s", subject);
            for (int i = 0; i < mcount; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (int j = 0; j < mails[i].tcount; j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    for (int j = i; j < mcount - 1; j++) mails[j] = mails[j+1];
                    mcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[100];
            scanf("%s", tag);
            for (int i = 0; i < mcount; i++) {
                for (int j = 0; j < mails[i].tcount; j++) {
                    if (strcmp(mails[i].tags[j], tag) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < mcount; i++) {
                printf("%s %s %s\n", mails[i].sender, mails[i].subject, mails[i].body);
            }
        }
    }
    for (int i = 0; i < mcount; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].tcount; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}