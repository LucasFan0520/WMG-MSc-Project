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

Message* find_by_subject(const char *subject) {
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

Tag* find_tag(Message *m, const char *tag) {
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_tag_to_message(Message *m, const char *tag) {
    if (find_tag(m, tag)) return;
    Tag *t = malloc(sizeof(Tag));
    t->tag = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void tag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (m) add_tag_to_message(m, tag);
}

void untag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags, *prev = NULL;
    while (t) {
        if (strcmp(t->tag, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->tag);
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
            Tag *t = m->tags;
            while (t) {
                Tag *tmp = t;
                t = t->next;
                free(tmp->tag);
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
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report_messages() {
    Message *m = messages;
    while (m) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
        m = m->next;
    }
}

void free_all() {
    while (messages) {
        Message *tmp = messages;
        messages = messages->next;
        free(tmp->sender);
        free(tmp->subject);
        free(tmp->body);
        Tag *t = tmp->tags;
        while (t) {
            Tag *ttmp = t;
            t = t->next;
            free(ttmp->tag);
            free(ttmp);
        }
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[4096];
        int n = sscanf(line, "%31s %255s %255s %[^\n]", cmd, a, b, c);
        if (strcmp(cmd, "MAIL") == 0 && n >= 3) {
            add_message(a, b, c);
        } else if (strcmp(cmd, "TAG") == 0 && n >= 3) {
            tag_message(a, b);
        } else if (strcmp(cmd, "UNTAG") == 0 && n >= 3) {
            untag_message(a, b);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_message(a);
        } else if (strcmp(cmd, "SEARCH") == 0 && n >= 2) {
            search_tag(a);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_messages();
        }
    }
    free_all();
    return 0;
}