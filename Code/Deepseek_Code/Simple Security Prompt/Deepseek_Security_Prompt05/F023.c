/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag {
    char *name;
    struct Tag *next;
} Tag;

typedef struct Message {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct Message *next;
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
    if (!m) return;
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

void tag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = malloc(sizeof(Tag));
    if (!t) return;
    t->name = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void untag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags, *prev = NULL;
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

void delete_message(const char *subject) {
    Message *m = messages, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else messages = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            while (m->tags) {
                Tag *tmp = m->tags;
                m->tags = m->tags->next;
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

void report_all(void) {
    Message *m = messages;
    while (m) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
        Tag *t = m->tags;
        while (t) {
            printf("%s\n", t->name);
            t = t->next;
        }
        m = m->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "MAIL") == 0 && n >= 3) {
            char *body = line + strlen(cmd) + strlen(arg1) + strlen(arg2) + 3;
            while (*body == ' ') body++;
            mail_message(arg1, arg2, body);
        } else if (strcmp(cmd, "TAG") == 0 && n >= 3) {
            tag_message(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0 && n >= 3) {
            untag_message(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_message(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0 && n >= 2) {
            search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (messages) {
        Message *tmp = messages;
        messages = messages->next;
        free(tmp->sender);
        free(tmp->subject);
        free(tmp->body);
        while (tmp->tags) {
            Tag *t = tmp->tags;
            tmp->tags = tmp->tags->next;
            free(t->name);
            free(t);
        }
        free(tmp);
    }
    return 0;
}