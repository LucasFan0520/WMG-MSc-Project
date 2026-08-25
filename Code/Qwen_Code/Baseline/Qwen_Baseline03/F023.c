// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char sender[100];
    char subject[100];
    char body[1000];
    char tags[100][100];
    int tcnt;
} Mail;

Mail mails[1000];
int mn = 0;

int main() {
    char cmd[20], sender[100], subject[100], body[1000], tag[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "MAIL")) {
            scanf("%s %s", sender, subject);
            fgets(body, 1000, stdin);
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            body[strcspn(body, "\n")] = 0;
            strcpy(mails[mn].sender, sender);
            strcpy(mails[mn].subject, subject);
            strcpy(mails[mn].body, body);
            mails[mn].tcnt = 0;
            mn++;
        } else if (!strcmp(cmd, "TAG")) {
            scanf("%s %s", subject, tag);
            for (int i = 0; i < mn; i++) {
                if (!strcmp(mails[i].subject, subject)) {
                    strcpy(mails[i].tags[mails[i].tcnt++], tag);
                    break;
                }
            }
        } else if (!strcmp(cmd, "UNTAG")) {
            scanf("%s %s", subject, tag);
            for (int i = 0; i < mn; i++) {
                if (!strcmp(mails[i].subject, subject)) {
                    for (int j = 0; j < mails[i].tcnt; j++) {
                        if (!strcmp(mails[i].tags[j], tag)) {
                            for (int k = j; k < mails[i].tcnt - 1; k++) {
                                strcpy(mails[i].tags[k], mails[i].tags[k + 1]);
                            }
                            mails[i].tcnt--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", subject);
            for (int i = 0; i < mn; i++) {
                if (!strcmp(mails[i].subject, subject)) {
                    for (int j = i; j < mn - 1; j++) {
                        mails[j] = mails[j + 1];
                    }
                    mn--;
                    break;
                }
            }
        } else if (!strcmp(cmd, "SEARCH")) {
            scanf("%s", tag);
            for (int i = 0; i < mn; i++) {
                for (int j = 0; j < mails[i].tcnt; j++) {
                    if (!strcmp(mails[i].tags[j], tag)) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < mn; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].tcnt; j++) {
                    printf(" %s", mails[i].tags[j]);
                }
                printf("\n");
            }
        }
    }
    return 0;
}