/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag {
    char *tag;
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

void add_message(const char *sender, const char *subject, const char *body) {
    Message *m = malloc(sizeof(Message));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

void add_tag(Message *m, const char *tag) {
    Tag *t = malloc(sizeof(Tag));
    t->tag = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void untag(Message *m, const char *tag) {
    Tag *prev = NULL, *cur = m->tags;
    while (cur) {
        if (strcmp(cur->tag, tag) == 0) {
            if (prev) prev->next = cur->next;
            else m->tags = cur->next;
            free(cur->tag);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_message(const char *subject) {
    Message *prev = NULL, *cur = messages;
    while (cur) {
        if (strcmp(cur->subject, subject) == 0) {
            if (prev) prev->next = cur->next;
            else messages = cur->next;
            free(cur->sender);
            free(cur->subject);
            free(cur->body);
            Tag *t = cur->tags;
            while (t) {
                Tag *tt = t;
                t = t->next;
                free(tt->tag);
                free(tt);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void search_tag(const char *tag) {
    Message *m = messages;
    while (m) {
        Tag *t = m->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report(void) {
    Message *m = messages;
    while (m) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
        m = m->next;
    }
}

void free_messages(void) {
    while (messages) {
        Message *tmp = messages;
        messages = messages->next;
        free(tmp->sender);
        free(tmp->subject);
        free(tmp->body);
        Tag *t = tmp->tags;
        while (t) {
            Tag *tt = t;
            t = t->next;
            free(tt->tag);
            free(tt);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char sender[256], subject[256], tag[256];
        int n = sscanf(line, "%s %s %s %s", cmd, sender, subject, tag);
        if (strcmp(cmd, "MAIL") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1 + strlen(sender) + 1 + strlen(subject) + 1;
            while (*p == ' ') p++;
            if (*p) add_message(sender, subject, p);
        } else if (strcmp(cmd, "TAG") == 0) {
            if (n < 3) continue;
            Message *m = find_by_subject(sender);
            if (m) add_tag(m, subject);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (n < 3) continue;
            Message *m = find_by_subject(sender);
            if (m) untag(m, subject);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_message(sender);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (n < 2) continue;
            search_tag(sender);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_messages();
    return 0;
}