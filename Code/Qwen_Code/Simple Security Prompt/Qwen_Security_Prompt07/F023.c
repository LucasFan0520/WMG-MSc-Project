// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag; } Tag;
typedef struct { char *sender, *subject, *body; Tag *tags; int n, cap; } Mail;
Mail *mails = 0; int n = 0, cap = 0;

int find_subj(char *subj) {
    for (int i = 0; i < n; i++) if (!strcmp(mails[i].subject, subj)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], rest[2048];
        a[0] = b[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "MAIL")) sscanf(line, "%*s %s %s %[^\n]", a, b, rest);
        else if (!strcmp(cmd, "TAG") || !strcmp(cmd, "UNTAG")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "MAIL")) {
            if (n == cap) { cap = cap ? cap * 2 : 4; mails = realloc(mails, cap * sizeof(Mail)); }
            mails[n].sender = strdup(a); mails[n].subject = strdup(b); mails[n].body = strdup(rest);
            mails[n].tags = 0; mails[n].n = 0; mails[n].cap = 0; n++;
        } else if (!strcmp(cmd, "TAG")) {
            int i = find_subj(a);
            if (i >= 0) {
                int exists = 0;
                for (int j = 0; j < mails[i].n; j++) if (!strcmp(mails[i].tags[j].tag, b)) { exists = 1; break; }
                if (!exists) {
                    if (mails[i].n == mails[i].cap) {
                        mails[i].cap = mails[i].cap ? mails[i].cap * 2 : 4;
                        mails[i].tags = realloc(mails[i].tags, mails[i].cap * sizeof(Tag));
                    }
                    mails[i].tags[mails[i].n++].tag = strdup(b);
                }
            }
        } else if (!strcmp(cmd, "UNTAG")) {
            int i = find_subj(a);
            if (i >= 0) {
                for (int j = 0; j < mails[i].n; j++) {
                    if (!strcmp(mails[i].tags[j].tag, b)) {
                        free(mails[i].tags[j].tag);
                        for (int k = j; k < mails[i].n - 1; k++) mails[i].tags[k] = mails[i].tags[k + 1];
                        mails[i].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int i = find_subj(a);
            if (i >= 0) {
                for (int j = 0; j < mails[i].n; j++) free(mails[i].tags[j].tag);
                free(mails[i].tags); free(mails[i].sender); free(mails[i].subject); free(mails[i].body);
                for (int j = i; j < n - 1; j++) mails[j] = mails[j + 1]; n--;
            }
        } else if (!strcmp(cmd, "SEARCH")) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < mails[i].n; j++) {
                    if (!strcmp(mails[i].tags[j].tag, a)) { puts(mails[i].subject); break; }
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < n; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].n; j++) printf(" %s", mails[i].tags[j].tag);
                putchar('\n');
            }
        }
    }
    return 0;
}