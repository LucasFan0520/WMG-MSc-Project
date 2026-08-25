// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int num_tags;
} Mail;

Mail *mails = NULL;
int num_mails = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256];
            sscanf(line, "%*s %s %s", sender, subject);
            char *body = strstr(line, subject);
            if (body) {
                body += strlen(subject);
                if (*body == ' ') body++;
            } else {
                body = "";
            }
            mails = realloc(mails, (num_mails + 1) * sizeof(Mail));
            mails[num_mails].sender = strdup(sender);
            mails[num_mails].subject = strdup(subject);
            mails[num_mails].body = strdup(body);
            mails[num_mails].tags = NULL;
            mails[num_mails].num_tags = 0;
            num_mails++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            sscanf(line, "%*s %s %s", subject, tag);
            for (int i = 0; i < num_mails; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    mails[i].tags = realloc(mails[i].tags, (mails[i].num_tags + 1) * sizeof(char*));
                    mails[i].tags[mails[i].num_tags++] = strdup(tag);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag[256];
            sscanf(line, "%*s %s %s", subject, tag);
            for (int i = 0; i < num_mails; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    for (int j = 0; j < mails[i].num_tags; j++) {
                        if (strcmp(mails[i].tags[j], tag) == 0) {
                            free(mails[i].tags[j]);
                            for (int k = j; k < mails[i].num_tags - 1; k++) {
                                mails[i].tags[k] = mails[i].tags[k + 1];
                            }
                            mails[i].num_tags--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256];
            sscanf(line, "%*s %s", subject);
            for (int i = 0; i < num_mails; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (int j = 0; j < mails[i].num_tags; j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    for (int j = i; j < num_mails - 1; j++) {
                        mails[j] = mails[j + 1];
                    }
                    num_mails--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256];
            sscanf(line, "%*s %s", tag);
            for (int i = 0; i < num_mails; i++) {
                for (int j = 0; j < mails[i].num_tags; j++) {
                    if (strcmp(mails[i].tags[j], tag) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < num_mails; i++) {
                printf("%s %s %s\n", mails[i].sender, mails[i].subject, mails[i].body);
            }
        }
    }
    
    for (int i = 0; i < num_mails; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].num_tags; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    free(mails);
    
    return 0;
}