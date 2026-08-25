// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char *tags[100];
    int tc;
} Mail;

Mail mails[1000];
int mc = 0;

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[1000], subject[1000];
            sscanf(rest, "%s %s", sender, subject);
            char *body = rest;
            while (*body && *body != ' ') body++;
            while (*body == ' ') body++;
            while (*body && *body != ' ') body++;
            while (*body == ' ') body++;
            mails[mc].sender = strdup(sender);
            mails[mc].subject = strdup(subject);
            mails[mc].body = strdup(body);
            mails[mc].tc = 0;
            mc++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[1000], tag[1000];
            sscanf(rest, "%s %s", subject, tag);
            for (int i = 0; i < mc; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    mails[i].tags[mails[i].tc++] = strdup(tag);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[1000], tag[1000];
            sscanf(rest, "%s %s", subject, tag);
            for (int i = 0; i < mc; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    for (int j = 0; j < mails[i].tc; j++) {
                        if (strcmp(mails[i].tags[j], tag) == 0) {
                            free(mails[i].tags[j]);
                            for (int k = j; k < mails[i].tc - 1; k++) mails[i].tags[k] = mails[i].tags[k+1];
                            mails[i].tc--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < mc; i++) {
                if (strcmp(mails[i].subject, rest) == 0) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (int j = 0; j < mails[i].tc; j++) free(mails[i].tags[j]);
                    for (int j = i; j < mc - 1; j++) mails[j] = mails[j+1];
                    mc--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            for (int i = 0; i < mc; i++) {
                for (int j = 0; j < mails[i].tc; j++) {
                    if (strcmp(mails[i].tags[j], rest) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < mc; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].tc; j++) printf(" [%s]", mails[i].tags[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < mc; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].tc; j++) free(mails[i].tags[j]);
    }
    return 0;
}