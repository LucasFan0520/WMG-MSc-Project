// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *sender; char *subject; char *body; char **tags; int ntags; } Mail;
Mail mails[1000]; int nmails = 0;

int main() {
    char cmd[20], sender[100], subject[100], body[1000], tag[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "MAIL")) {
            scanf("%s", sender);
            fgets(subject, sizeof(subject), stdin);
            subject[strcspn(subject, "\n")] = 0;
            if (subject[0] == ' ') memmove(subject, subject + 1, strlen(subject));
            char *space = strchr(subject, ' ');
            if (space) {
                *space = 0;
                char *b = space + 1;
                while(*b == ' ') b++;
                mails[nmails].sender = strdup(sender);
                mails[nmails].subject = strdup(subject);
                mails[nmails].body = strdup(b);
                mails[nmails].tags = NULL;
                mails[nmails++].ntags = 0;
            }
        } else if (!strcmp(cmd, "TAG")) {
            scanf("%s %s", subject, tag);
            for (int i = 0; i < nmails; i++) {
                if (!strcmp(mails[i].subject, subject)) {
                    int found = 0;
                    for (int j = 0; j < mails[i].ntags; j++) if (!strcmp(mails[i].tags[j], tag)) found = 1;
                    if (!found) {
                        mails[i].tags = realloc(mails[i].tags, sizeof(char*) * (mails[i].ntags + 1));
                        mails[i].tags[mails[i].ntags++] = strdup(tag);
                    }
                    break;
                }
            }
        } else if (!strcmp(cmd, "UNTAG")) {
            scanf("%s %s", subject, tag);
            for (int i = 0; i < nmails; i++) {
                if (!strcmp(mails[i].subject, subject)) {
                    for (int j = 0; j < mails[i].ntags; j++) {
                        if (!strcmp(mails[i].tags[j], tag)) {
                            free(mails[i].tags[j]);
                            for (int k = j; k < mails[i].ntags - 1; k++) mails[i].tags[k] = mails[i].tags[k + 1];
                            mails[i].ntags--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            fgets(subject, sizeof(subject), stdin);
            subject[strcspn(subject, "\n")] = 0;
            if (subject[0] == ' ') memmove(subject, subject + 1, strlen(subject));
            for (int i = 0; i < nmails; i++) {
                if (!strcmp(mails[i].subject, subject)) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (int j = 0; j < mails[i].ntags; j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    for (int j = i; j < nmails - 1; j++) mails[j] = mails[j + 1];
                    nmails--;
                    break;
                }
            }
        } else if (!strcmp(cmd, "SEARCH")) {
            scanf("%s", tag);
            for (int i = 0; i < nmails; i++) {
                for (int j = 0; j < mails[i].ntags; j++) {
                    if (!strcmp(mails[i].tags[j], tag)) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nmails; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].ntags; j++) printf(" %s", mails[i].tags[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < nmails; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].ntags; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    return 0;
}