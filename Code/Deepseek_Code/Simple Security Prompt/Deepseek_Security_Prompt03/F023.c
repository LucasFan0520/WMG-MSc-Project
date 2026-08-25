/*
 * F023.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tag {
    char *name;
    struct tag *next;
} Tag;

typedef struct message {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct message *next;
} Message;

Message *messages = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void mail_message(const char *sender, const char *subject, const char *body) {
    if (find_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

void tag_subject(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->name, tag) == 0) return;
        t = t->next;
    }
    Tag *nt = malloc(sizeof(Tag));
    nt->name = strdup(tag);
    nt->next = m->tags;
    m->tags = nt;
}

void untag_subject(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL, *t = m->tags;
    while (t) {
        if (strcmp(t->name, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void delete_subject(const char *subject) {
    Message *prev = NULL, *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else messages = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            Tag *t = m->tags;
            while (t) {
                Tag *tmp = t;
                t = t->next;
                free(tmp->name);
                free(tmp);
            }
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_tag(const char *tag) {
    Message *m = messages;
    while (m) {
        Tag *t = m->tags;
        while (t) {
            if (strcmp(t->name, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report_messages(void) {
    Message *m = messages;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        Tag *t = m->tags;
        while (t) {
            printf(" %s", t->name);
            t = t->next;
        }
        printf("\n");
        m = m->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256];
        if (sscanf(line, "%31s %255s %255s", cmd, arg1, arg2) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "MAIL") == 0) {
            char *subject_start = rest;
            char *body_start = subject_start + strlen(arg1) + 1;
            while (*body_start == ' ') body_start++;
            mail_message(arg1, subject_start, body_start);
        } else if (strcmp(cmd, "TAG") == 0) {
            tag_subject(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            untag_subject(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_subject(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    return 0;
}