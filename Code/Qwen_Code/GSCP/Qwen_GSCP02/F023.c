/* F023.c */
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
    int tcount;
    int tcap;
} Mail;

static Mail *mails = NULL;
static int mcount = 0;
static int mcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_first_subject(const char *subject) {
    for (int i = 0; i < mcount; i++)
        if (strcmp(mails[i].subject, subject) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256] = {0}, subject[256] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, "%255s%n %255s%n", sender, &off2, subject, &off3) != 2) continue;
            const char *body = line + offset + off2 + off3;
            while (*body == ' ') body++;
            if (mcount >= mcap) {
                mcap = mcap == 0 ? 8 : mcap * 2;
                mails = realloc(mails, sizeof(Mail) * mcap);
            }
            mails[mcount].sender = dup_str(sender);
            mails[mcount].subject = dup_str(subject);
            mails[mcount].body = dup_str(body);
            mails[mcount].tags = NULL;
            mails[mcount].tcount = 0;
            mails[mcount].tcap = 0;
            mcount++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256] = {0}, tag[256] = {0};
            if (sscanf(line + offset, "%255s %255s", subject, tag) != 2) continue;
            int idx = find_first_subject(subject);
            if (idx < 0) continue;
            Mail *m = &mails[idx];
            if (m->tcount >= m->tcap) {
                m->tcap = m->tcap == 0 ? 4 : m->tcap * 2;
                m->tags = realloc(m->tags, sizeof(Tag) * m->tcap);
            }
            m->tags[m->tcount++].tag = dup_str(tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256] = {0}, tag[256] = {0};
            if (sscanf(line + offset, "%255s %255s", subject, tag) != 2) continue;
            int idx = find_first_subject(subject);
            if (idx < 0) continue;
            Mail *m = &mails[idx];
            for (int i = 0; i < m->tcount; i++) {
                if (strcmp(m->tags[i].tag, tag) == 0) {
                    free(m->tags[i].tag);
                    for (int j = i; j < m->tcount - 1; j++) m->tags[j] = m->tags[j + 1];
                    m->tcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256] = {0};
            if (sscanf(line + offset, "%255s", subject) != 1) continue;
            int idx = find_first_subject(subject);
            if (idx < 0) continue;
            free(mails[idx].sender);
            free(mails[idx].subject);
            free(mails[idx].body);
            for (int i = 0; i < mails[idx].tcount; i++) free(mails[idx].tags[i].tag);
            free(mails[idx].tags);
            for (int i = idx; i < mcount - 1; i++) mails[i] = mails[i + 1];
            mcount--;
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256] = {0};
            if (sscanf(line + offset, "%255s", tag) != 1) continue;
            for (int i = 0; i < mcount; i++) {
                for (int j = 0; j < mails[i].tcount; j++) {
                    if (strcmp(mails[i].tags[j].tag, tag) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < mcount; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].tcount; j++) printf(" [%s]", mails[i].tags[j].tag);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < mcount; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].tcount; j++) free(mails[i].tags[j].tag);
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}