// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
} Tag;

typedef struct {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    int tag_count;
    int tag_cap;
} Mail;

Mail *mails = NULL;
int m_count = 0;

int find_subject(const char *subject) {
    for (int i = 0; i < m_count; i++) {
        if (strcmp(mails[i].subject, subject) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char sender[1024];
    char subject[1024];
    char body[10240];
    char tag[1024];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "MAIL") == 0) {
            scanf("%s %s", sender, subject);
            fgets(body, sizeof(body), stdin);
            if (body[0] == ' ') {
                memmove(body, body + 1, strlen(body));
            }
            body[strcspn(body, "\n")] = '\0';
            mails = realloc(mails, sizeof(Mail) * (m_count + 1));
            mails[m_count].sender = strdup(sender);
            mails[m_count].subject = strdup(subject);
            mails[m_count].body = strdup(body);
            mails[m_count].tags = NULL;
            mails[m_count].tag_count = 0;
            mails[m_count].tag_cap = 0;
            m_count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            scanf("%s %s", subject, tag);
            int idx = find_subject(subject);
            if (idx != -1) {
                if (mails[idx].tag_count == mails[idx].tag_cap) {
                    mails[idx].tag_cap = mails[idx].tag_cap ? mails[idx].tag_cap * 2 : 4;
                    mails[idx].tags = realloc(mails[idx].tags, sizeof(Tag) * mails[idx].tag_cap);
                }
                mails[idx].tags[mails[idx].tag_count].tag = strdup(tag);
                mails[idx].tag_count++;
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            scanf("%s %s", subject, tag);
            int idx = find_subject(subject);
            if (idx != -1) {
                for (int i = 0; i < mails[idx].tag_count; i++) {
                    if (strcmp(mails[idx].tags[i].tag, tag) == 0) {
                        free(mails[idx].tags[i].tag);
                        for (int j = i; j < mails[idx].tag_count - 1; j++) {
                            mails[idx].tags[j] = mails[idx].tags[j + 1];
                        }
                        mails[idx].tag_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", subject);
            int idx = find_subject(subject);
            if (idx != -1) {
                free(mails[idx].sender);
                free(mails[idx].subject);
                free(mails[idx].body);
                for (int i = 0; i < mails[idx].tag_count; i++) {
                    free(mails[idx].tags[i].tag);
                }
                free(mails[idx].tags);
                for (int i = idx; i < m_count - 1; i++) {
                    mails[i] = mails[i + 1];
                }
                m_count--;
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            scanf("%s", tag);
            for (int i = 0; i < m_count; i++) {
                for (int j = 0; j < mails[i].tag_count; j++) {
                    if (strcmp(mails[i].tags[j].tag, tag) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < m_count; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].tag_count; j++) {
                    printf(" [%s]", mails[i].tags[j].tag);
                }
                printf("\n");
            }
        }
    }

    for (int i = 0; i < m_count; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].tag_count; j++) {
            free(mails[i].tags[j].tag);
        }
        free(mails[i].tags);
    }
    free(mails);

    return 0;
}