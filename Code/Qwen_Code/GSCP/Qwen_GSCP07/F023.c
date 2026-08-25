// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag {
    char *name;
    struct Tag *next;
} Tag;

typedef struct Mail {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct Mail *next;
} Mail;

Mail *mails = NULL;

void add_mail(const char *sender, const char *subject, const char *body) {
    Mail *m = malloc(sizeof(Mail));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = mails;
    mails = m;
}

void add_tag(const char *subject, const char *tag) {
    for (Mail *m = mails; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            for (Tag *t = m->tags; t; t = t->next) {
                if (strcmp(t->name, tag) == 0) return;
            }
            Tag *t = malloc(sizeof(Tag));
            t->name = strdup(tag);
            t->next = m->tags;
            m->tags = t;
            return;
        }
    }
}

void remove_tag(const char *subject, const char *tag) {
    for (Mail *m = mails; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            Tag **tp = &m->tags;
            while (*tp) {
                if (strcmp((*tp)->name, tag) == 0) {
                    Tag *tmp = *tp;
                    *tp = tmp->next;
                    free(tmp->name);
                    free(tmp);
                    return;
                }
                tp = &(*tp)->next;
            }
            return;
        }
    }
}

void delete_mail(const char *subject) {
    Mail **mp = &mails;
    while (*mp) {
        if (strcmp((*mp)->subject, subject) == 0) {
            Mail *tmp = *mp;
            *mp = tmp->next;
            Tag *t = tmp->tags;
            while (t) {
                Tag *next_t = t->next;
                free(t->name);
                free(t);
                t = next_t;
            }
            free(tmp->sender);
            free(tmp->subject);
            free(tmp->body);
            free(tmp);
            return;
        }
        mp = &(*mp)->next;
    }
}

void search_tag(const char *tag) {
    for (Mail *m = mails; m; m = m->next) {
        for (Tag *t = m->tags; t; t = t->next) {
            if (strcmp(t->name, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

void report() {
    for (Mail *m = mails; m; m = m->next) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        for (Tag *t = m->tags; t; t = t->next) {
            printf(" %s", t->name);
        }
        printf("\n");
    }
}

void cleanup() {
    while (mails) {
        Mail *tmp = mails->next;
        Tag *t = mails->tags;
        while (t) {
            Tag *next_t = t->next;
            free(t->name);
            free(t);
            t = next_t;
        }
        free(mails->sender);
        free(mails->subject);
        free(mails->body);
        free(mails);
        mails = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) add_mail(a1, a2, t + 1); } }
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) add_tag(a1, a2);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) remove_tag(a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_mail(a1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) search_tag(a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    cleanup();
    return 0;
}