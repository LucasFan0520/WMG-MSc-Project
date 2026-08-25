// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

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

Mail* find_mail_by_subject(const char *subject) {
    for (Mail *m = mails; m; m = m->next)
        if (strcmp(m->subject, subject) == 0) return m;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256];
            int n = 0;
            if (sscanf(rest, "%255s %255s%n", sender, subject, &n) != 2) continue;
            char *body = rest + n;
            while (*body == ' ') body++;
            Mail *m = malloc(sizeof(Mail));
            if (!m) continue;
            m->sender = strdup(sender);
            m->subject = strdup(subject);
            m->body = strdup(body);
            if (!m->sender || !m->subject || !m->body) {
                free(m->sender); free(m->subject); free(m->body); free(m); continue;
            }
            m->tags = NULL;
            m->next = mails;
            mails = m;
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag_name[256];
            if (sscanf(rest, "%255s %255s", subject, tag_name) != 2) continue;
            Mail *m = find_mail_by_subject(subject);
            if (m) {
                Tag *t = malloc(sizeof(Tag));
                if (!t) continue;
                t->name = strdup(tag_name);
                if (!t->name) { free(t); continue; }
                t->next = m->tags;
                m->tags = t;
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag_name[256];
            if (sscanf(rest, "%255s %255s", subject, tag_name) != 2) continue;
            Mail *m = find_mail_by_subject(subject);
            if (m) {
                Tag *prev = NULL;
                for (Tag *t = m->tags; t; prev = t, t = t->next) {
                    if (strcmp(t->name, tag_name) == 0) {
                        if (prev) prev->next = t->next;
                        else m->tags = t->next;
                        free(t->name);
                        free(t);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256];
            if (sscanf(rest, "%255s", subject) != 1) continue;
            Mail *prev = NULL;
            for (Mail *m = mails; m; prev = m, m = m->next) {
                if (strcmp(m->subject, subject) == 0) {
                    if (prev) prev->next = m->next;
                    else mails = m->next;
                    while (m->tags) {
                        Tag *t = m->tags;
                        m->tags = t->next;
                        free(t->name);
                        free(t);
                    }
                    free(m->sender);
                    free(m->subject);
                    free(m->body);
                    free(m);
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag_name[256];
            if (sscanf(rest, "%255s", tag_name) != 1) continue;
            for (Mail *m = mails; m; m = m->next) {
                for (Tag *t = m->tags; t; t = t->next) {
                    if (strcmp(t->name, tag_name) == 0) {
                        printf("%s\n", m->subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Mail *m = mails; m; m = m->next) {
                printf("%s %s ", m->sender, m->subject);
                print_text(m->body);
            }
        }
    }
    while (mails) {
        Mail *m = mails;
        mails = mails->next;
        while (m->tags) {
            Tag *t = m->tags;
            m->tags = t->next;
            free(t->name);
            free(t);
        }
        free(m->sender);
        free(m->subject);
        free(m->body);
        free(m);
    }
    return 0;
}