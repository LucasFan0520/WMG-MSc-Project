// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tagc;
} Mail;

Mail *mails = NULL;
int mc = 0;

int find_mail_by_subject(const char *subject) {
    for (int i = 0; i < mc; i++) {
        if (strcmp(mails[i].subject, subject) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char rest[2048];
        a1[0] = '\0';
        a2[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %255s %2047[^\n]", cmd, a1, a2, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "MAIL") == 0 && n >= 4) {
            mails = realloc(mails, (mc + 1) * sizeof(Mail));
            if (!mails) return 1;
            mails[mc].sender = strdup(a1);
            mails[mc].subject = strdup(a2);
            mails[mc].body = strdup(rest);
            mails[mc].tags = NULL;
            mails[mc].tagc = 0;
            mc++;
        } else if (strcmp(cmd, "TAG") == 0 && n >= 3) {
            int mi = find_mail_by_subject(a1);
            if (mi >= 0) {
                mails[mi].tags = realloc(mails[mi].tags, (mails[mi].tagc + 1) * sizeof(char *));
                if (!mails[mi].tags) return 1;
                mails[mi].tags[mails[mi].tagc] = strdup(a2);
                mails[mi].tagc++;
            }
        } else if (strcmp(cmd, "UNTAG") == 0 && n >= 3) {
            int mi = find_mail_by_subject(a1);
            if (mi >= 0) {
                for (int j = 0; j < mails[mi].tagc; j++) {
                    if (strcmp(mails[mi].tags[j], a2) == 0) {
                        free(mails[mi].tags[j]);
                        for (int k = j; k < mails[mi].tagc - 1; k++) {
                            mails[mi].tags[k] = mails[mi].tags[k + 1];
                        }
                        mails[mi].tagc--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int mi = find_mail_by_subject(a1);
            if (mi >= 0) {
                free(mails[mi].sender);
                free(mails[mi].subject);
                free(mails[mi].body);
                for (int j = 0; j < mails[mi].tagc; j++) {
                    free(mails[mi].tags[j]);
                }
                free(mails[mi].tags);
                for (int k = mi; k < mc - 1; k++) {
                    mails[k] = mails[k + 1];
                }
                mc--;
            }
        } else if (strcmp(cmd, "SEARCH") == 0 && n >= 2) {
            for (int i = 0; i < mc; i++) {
                for (int j = 0; j < mails[i].tagc; j++) {
                    if (strcmp(mails[i].tags[j], a1) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < mc; i++) {
                printf("%s %s %s\n", mails[i].sender, mails[i].subject, mails[i].body);
            }
        }
    }
    for (int i = 0; i < mc; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].tagc; j++) {
            free(mails[i].tags[j]);
        }
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}