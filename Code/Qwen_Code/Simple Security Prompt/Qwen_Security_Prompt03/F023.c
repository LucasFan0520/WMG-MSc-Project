// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *sender; char *subject; char *body; char **tags; int ntags; } Mail;
Mail *mails = NULL;
int nm = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "MAIL") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            char *body = strchr(t, ' ');
            char subj[256];
            strncpy(subj, t, body ? body - t : strlen(t));
            subj[body ? body - t : strlen(t)] = 0;
            
            mails = realloc(mails, (nm + 1) * sizeof(Mail));
            mails[nm].sender = strdup(a);
            mails[nm].subject = strdup(subj);
            mails[nm].body = strdup(body ? body + 1 : "");
            mails[nm].tags = NULL;
            mails[nm].ntags = 0;
            nm++;
        } else if (strcmp(cmd, "TAG") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            for (int i = 0; i < nm; i++) {
                if (strcmp(mails[i].subject, a) == 0) {
                    mails[i].tags = realloc(mails[i].tags, (mails[i].ntags + 1) * sizeof(char*));
                    mails[i].tags[mails[i].ntags++] = strdup(b);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            for (int i = 0; i < nm; i++) {
                if (strcmp(mails[i].subject, a) == 0) {
                    for (int j = 0; j < mails[i].ntags; j++) {
                        if (strcmp(mails[i].tags[j], b) == 0) {
                            free(mails[i].tags[j]);
                            for (int k = j; k < mails[i].ntags - 1; k++) mails[i].tags[k] = mails[i].tags[k + 1];
                            mails[i].ntags--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nm; i++) {
                if (strcmp(mails[i].subject, a) == 0) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (int j = 0; j < mails[i].ntags; j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    for (int j = i; j < nm - 1; j++) mails[j] = mails[j + 1];
                    nm--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nm; i++) {
                for (int j = 0; j < mails[i].ntags; j++) {
                    if (strcmp(mails[i].tags[j], a) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nm; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].ntags; j++) printf(" [%s]", mails[i].tags[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < nm; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].ntags; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}