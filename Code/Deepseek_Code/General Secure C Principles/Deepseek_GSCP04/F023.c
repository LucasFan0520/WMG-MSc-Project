/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Mail {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t tcount, tcap;
} Mail;

static Mail *mails = NULL;
static size_t mcount = 0, mcap = 0;

static Mail *find_mail_by_subject(const char *subject) {
    for (size_t i = 0; i < mcount; i++) {
        if (strcmp(mails[i].subject, subject) == 0)
            return &mails[i];
    }
    return NULL;
}

static void add_mail(const char *sender, const char *subject, const char *body) {
    if (mcount == mcap) {
        mcap = mcap ? mcap * 2 : 4;
        mails = realloc(mails, mcap * sizeof(Mail));
        if (!mails) { perror("realloc"); exit(1); }
    }
    mails[mcount].sender = strdup(sender);
    mails[mcount].subject = strdup(subject);
    mails[mcount].body = strdup(body);
    mails[mcount].tags = NULL;
    mails[mcount].tcount = mails[mcount].tcap = 0;
    mcount++;
}

static void add_tag(Mail *m, const char *tag) {
    if (m->tcount == m->tcap) {
        m->tcap = m->tcap ? m->tcap * 2 : 4;
        m->tags = realloc(m->tags, m->tcap * sizeof(char *));
        if (!m->tags) { perror("realloc"); exit(1); }
    }
    m->tags[m->tcount] = strdup(tag);
    m->tcount++;
}

static void remove_tag(Mail *m, const char *tag) {
    for (size_t i = 0; i < m->tcount; i++) {
        if (strcmp(m->tags[i], tag) == 0) {
            free(m->tags[i]);
            for (size_t j = i; j < m->tcount - 1; j++)
                m->tags[j] = m->tags[j + 1];
            m->tcount--;
            return;
        }
    }
}

static void delete_mail_by_subject(const char *subject) {
    for (size_t i = 0; i < mcount; i++) {
        if (strcmp(mails[i].subject, subject) == 0) {
            free(mails[i].sender);
            free(mails[i].subject);
            free(mails[i].body);
            for (size_t j = 0; j < mails[i].tcount; j++) free(mails[i].tags[j]);
            free(mails[i].tags);
            for (size_t j = i; j < mcount - 1; j++)
                mails[j] = mails[j + 1];
            mcount--;
            return;
        }
    }
}

static void search_tag(const char *tag) {
    for (size_t i = 0; i < mcount; i++) {
        for (size_t j = 0; j < mails[i].tcount; j++) {
            if (strcmp(mails[i].tags[j], tag) == 0) {
                printf("%s\n", mails[i].subject);
                break;
            }
        }
    }
}

static void report_mails(void) {
    for (size_t i = 0; i < mcount; i++) {
        printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
        for (size_t j = 0; j < mails[i].tcount; j++) {
            printf(" %s", mails[i].tags[j]);
        }
        printf("\n");
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = strtok(NULL, " ");
            if (!sender) continue;
            char *subject = strtok(NULL, " ");
            if (!subject) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            add_mail(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = strtok(NULL, " ");
            if (!subject) continue;
            char *tag = strtok(NULL, " ");
            if (!tag) continue;
            Mail *m = find_mail_by_subject(subject);
            if (m) add_tag(m, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = strtok(NULL, " ");
            if (!subject) continue;
            char *tag = strtok(NULL, " ");
            if (!tag) continue;
            Mail *m = find_mail_by_subject(subject);
            if (m) remove_tag(m, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = strtok(NULL, " ");
            if (subject) delete_mail_by_subject(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_mails();
        }
    }
    for (size_t i = 0; i < mcount; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (size_t j = 0; j < mails[i].tcount; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}