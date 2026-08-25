// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *sender; char *subject; char *body; char **tags; size_t tgc; size_t tgcap; } Mail;
Mail *mails = NULL;
size_t mc = 0, mcap = 0;

Mail* find_mail_subj(const char *subj) {
    for (size_t i = 0; i < mc; i++) if (strcmp(mails[i].subject, subj) == 0) return &mails[i];
    return NULL;
}

void add_mail(const char *sender, const char *subj, const char *body) {
    if (mc == mcap) {
        size_t nc = mcap ? mcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Mail)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Mail *tmp = realloc(mails, nc * sizeof(Mail));
        if (!tmp) { perror("realloc"); exit(1); }
        mails = tmp;
        mcap = nc;
    }
    mails[mc].sender = strdup(sender);
    mails[mc].subject = strdup(subj);
    mails[mc].body = strdup(body);
    mails[mc].tags = NULL;
    mails[mc].tgc = 0;
    mails[mc].tgcap = 0;
    if (!mails[mc].sender || !mails[mc].subject || !mails[mc].body) { perror("strdup"); exit(1); }
    mc++;
}

void add_tag(Mail *m, const char *tag) {
    if (m->tgc == m->tgcap) {
        size_t nc = m->tgcap ? m->tgcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(char*)) { fprintf(stderr, "Overflow\n"); exit(1); }
        char **tmp = realloc(m->tags, nc * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        m->tags = tmp;
        m->tgcap = nc;
    }
    m->tags[m->tgc] = strdup(tag);
    if (!m->tags[m->tgc]) { perror("strdup"); exit(1); }
    m->tgc++;
}

void rem_tag(Mail *m, const char *tag) {
    for (size_t i = 0; i < m->tgc; i++) {
        if (strcmp(m->tags[i], tag) == 0) {
            free(m->tags[i]);
            m->tags[i] = m->tags[--m->tgc];
            return;
        }
    }
}

void del_mail(const char *subj) {
    for (size_t i = 0; i < mc; i++) {
        if (strcmp(mails[i].subject, subj) == 0) {
            free(mails[i].sender); free(mails[i].subject); free(mails[i].body);
            for (size_t j = 0; j < mails[i].tgc; j++) free(mails[i].tags[j]);
            free(mails[i].tags);
            mails[i] = mails[--mc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subj[256];
            if (sscanf(line, "%*s %255s %255s", sender, subj) == 2) {
                char *body = strchr(line, ' ');
                if (body) { body = strchr(body + 1, ' '); if (body) body = strchr(body + 1, ' '); if (body) body++; }
                if (!body) body = "";
                add_mail(sender, subj, body);
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subj[256], tag[256];
            if (sscanf(line, "%*s %255s %255s", subj, tag) == 2) {
                Mail *m = find_mail_subj(subj);
                if (m) add_tag(m, tag);
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subj[256], tag[256];
            if (sscanf(line, "%*s %255s %255s", subj, tag) == 2) {
                Mail *m = find_mail_subj(subj);
                if (m) rem_tag(m, tag);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subj[256];
            if (sscanf(line, "%*s %255s", subj) == 1) del_mail(subj);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256];
            if (sscanf(line, "%*s %255s", tag) == 1) {
                for (size_t i = 0; i < mc; i++) {
                    for (size_t j = 0; j < mails[i].tgc; j++) {
                        if (strcmp(mails[i].tags[j], tag) == 0) {
                            printf("%s\n", mails[i].subject);
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < mc; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (size_t j = 0; j < mails[i].tgc; j++) printf(" [%s]", mails[i].tags[j]);
                printf("\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < mc; i++) {
        free(mails[i].sender); free(mails[i].subject); free(mails[i].body);
        for (size_t j = 0; j < mails[i].tgc; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}