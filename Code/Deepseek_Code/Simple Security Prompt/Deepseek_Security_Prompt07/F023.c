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

Message *messages = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

Tag *find_tag_in_message(Message *m, const char *tag) {
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_message(const char *sender, const char *subject, const char *body) {
    if (find_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
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
    if (find_tag_in_message(m, tag)) return;
    Tag *t = malloc(sizeof(Tag));
    t->tag = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void untag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL;
    Tag *t = m->tags;
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
    Message *prev = NULL;
    Message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else messages = m->next;
            Tag *t = m->tags;
            while (t) {
                Tag *next = t->next;
                free(t->tag);
                free(t);
                t = next;
            }
            free(m->sender);
            free(m->subject);
            free(m->body);
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_tag(const char *tag) {
    Message *m = messages;
    int first = 1;
    while (m) {
        Tag *t = m->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                if (!first) putchar(' ');
                printf("%s", m->subject);
                first = 0;
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
    putchar('\n');
}

void report_messages() {
    Message *m = messages;
    while (m) {
        printf("%s %s:", m->sender, m->subject);
        for (char *s = m->body; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar(' ');
        Tag *t = m->tags;
        int first = 1;
        while (t) {
            if (!first) putchar(' ');
            printf("%s", t->tag);
            first = 0;
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all() {
    Message *m = messages;
    while (m) {
        Message *nextm = m->next;
        Tag *t = m->tags;
        while (t) {
            Tag *nextt = t->next;
            free(t->tag);
            free(t);
            t = nextt;
        }
        free(m->sender);
        free(m->subject);
        free(m->body);
        free(m);
        m = nextm;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256], body[4096];
            if (sscanf(rest, "%255s %255s %[^\n]", sender, subject, body) == 3) {
                add_message(sender, subject, body);
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(rest, "%255s %255s", subject, tag) == 2) {
                tag_message(subject, tag);
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(rest, "%255s %255s", subject, tag) == 2) {
                untag_message(subject, tag);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_message(rest);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            search_tag(rest);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free_all();
    return 0;
}