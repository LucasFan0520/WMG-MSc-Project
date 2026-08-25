// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SENDER 100
#define MAX_SUBJ 100
#define MAX_BODY 1000
#define MAX_TAG 100

typedef struct {
    char sender[MAX_SENDER];
    char subject[MAX_SUBJ];
    char body[MAX_BODY];
    char tags[100][MAX_TAG];
    int tag_count;
} Mail;

Mail mails[1000];
int mail_count = 0;

int main() {
    char cmd[20], sender[MAX_SENDER], subject[MAX_SUBJ], body[MAX_BODY], tag[MAX_TAG];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "MAIL") == 0) {
            scanf("%s %s", sender, subject);
            fgets(body, MAX_BODY, stdin);
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            if (body[strlen(body) - 1] == '\n') body[strlen(body) - 1] = '\0';
            strcpy(mails[mail_count].sender, sender);
            strcpy(mails[mail_count].subject, subject);
            strcpy(mails[mail_count].body, body);
            mails[mail_count].tag_count = 0;
            mail_count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            scanf("%s %s", subject, tag);
            for (int i = 0; i < mail_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    strcpy(mails[i].tags[mails[i].tag_count], tag);
                    mails[i].tag_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            scanf("%s %s", subject, tag);
            for (int i = 0; i < mail_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    for (int j = 0; j < mails[i].tag_count; j++) {
                        if (strcmp(mails[i].tags[j], tag) == 0) {
                            for (int k = j; k < mails[i].tag_count - 1; k++) {
                                strcpy(mails[i].tags[k], mails[i].tags[k + 1]);
                            }
                            mails[i].tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", subject);
            for (int i = 0; i < mail_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    for (int j = i; j < mail_count - 1; j++) mails[j] = mails[j + 1];
                    mail_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            scanf("%s", tag);
            for (int i = 0; i < mail_count; i++) {
                for (int j = 0; j < mails[i].tag_count; j++) {
                    if (strcmp(mails[i].tags[j], tag) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < mail_count; i++) {
                printf("%s %s %s\n", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].tag_count; j++) printf("  %s\n", mails[i].tags[j]);
            }
        }
    }
    return 0;
}