// F023.c
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

Message *head = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *sender, const char *subject, const char *body) {
    if (find_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = NULL;
    if (!m->sender || !m->subject || !m->body) {
        free(m->sender); free(m->subject); free(m->body); free(m);
        return;
    }
    if (!head) head = m;
    else {
        Message *last = head;
        while (last->next) last = last->next;
        last->next = m;
    }
}

void tag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return;
        t = t->next;
    }
    Tag *nt = malloc(sizeof(Tag));
    if (!nt) return;
    nt->tag = strdup(tag);
    nt->next = NULL;
    if (!nt->tag) { free(nt); return; }
    if (!m->tags) m->tags = nt;
    else {
        Tag *last = m->tags;
        while (last->next) last = last->next;
        last->next = nt;
    }
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
    Message *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            Tag *t = m->tags;
            while (t) {
                Tag *next = t->next;
                free(t->tag);
                free(t);
                t = next;
            }
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_tag(const char *tag) {
    Message *m = head;
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

void report_messages(void) {
    Message *m = head;
    while (m) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
        Tag *t = m->tags;
        while (t) {
            printf("%s\n", t->tag);
            t = t->next;
        }
        m = m->next;
    }
}

void free_all(void) {
    Message *m = head;
    while (m) {
        Message *next = m->next;
        free(m->sender);
        free(m->subject);
        free(m->body);
        Tag *t = m->tags;
        while (t) {
            Tag *tnext = t->next;
            free(t->tag);
            free(t);
            t = tnext;
        }
        free(m);
        m = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], sender[256], subject[256], tag[256], body[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", sender, subject, body) == 3)
                add_message(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2)
                tag_message(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2)
                untag_message(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", subject) == 1)
                delete_message(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(line, "%*s %255s", tag) == 1)
                search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free(line);
    free_all();
    return 0;
}