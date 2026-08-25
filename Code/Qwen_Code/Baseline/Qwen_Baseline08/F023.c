// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MAIL 1000
#define MAX_TAG 100
#define MAX_LEN 1000

typedef struct {
    char sender[100];
    char subject[100];
    char body[MAX_LEN];
    char tags[MAX_TAG][100];
    int tcount;
} Mail;

Mail mails[MAX_MAIL];
int mcount = 0;

int find_mail_subj(const char *subj) {
    for (int i = 0; i < mcount; i++) if (strcmp(mails[i].subject, subj) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], sender[100], subj[100], body[MAX_LEN], tag[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "MAIL") == 0) {
            scanf("%s %s", sender, subj);
            fgets(body, MAX_LEN, stdin);
            body[strcspn(body, "\n")] = 0;
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            if (mcount < MAX_MAIL) {
                strcpy(mails[mcount].sender, sender);
                strcpy(mails[mcount].subject, subj);
                strcpy(mails[mcount].body, body);
                mails[mcount].tcount = 0;
                mcount++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            scanf("%s %s", subj, tag);
            int idx = find_mail_subj(subj);
            if (idx != -1 && mails[idx].tcount < MAX_TAG) {
                strcpy(mails[idx].tags[mails[idx].tcount++], tag);
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            scanf("%s %s", subj, tag);
            int idx = find_mail_subj(subj);
            if (idx != -1) {
                for (int i = 0; i < mails[idx].tcount; i++) {
                    if (strcmp(mails[idx].tags[i], tag) == 0) {
                        for (int j = i; j < mails[idx].tcount - 1; j++) {
                            strcpy(mails[idx].tags[j], mails[idx].tags[j + 1]);
                        }
                        mails[idx].tcount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", subj);
            int idx = find_mail_subj(subj);
            if (idx != -1) {
                for (int i = idx; i < mcount - 1; i++) mails[i] = mails[i + 1];
                mcount--;
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
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
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].tcount; j++) printf(" [%s]", mails[i].tags[j]);
                printf("\n");
            }
        }
    }
    return 0;
}