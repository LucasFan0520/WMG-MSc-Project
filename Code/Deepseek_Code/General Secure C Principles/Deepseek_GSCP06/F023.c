// F023.c
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

void add_tag(const char *subject, const char *tag) {
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

void remove_tag(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *cur = m->tags, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, tag) == 0) {
            if (prev) prev->next = cur->next;
            else m->tags = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_message(const char *subject) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Message *cur = messages, *prev = NULL;
    while (cur) {
        if (cur == m) {
            if (prev) prev->next = cur->next;
            else messages = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
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
}

void search_tag(const char *tag) {
    Message *m = messages;
    int first = 1;
    while (m) {
        Tag *t = m->tags;
        while (t) {
            if (strcmp(t->name, tag) == 0) {
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

void report_messages(void) {
    Message *m = messages;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        Tag *t = m->tags;
        while (t) {
            printf(" %s", t->name);
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

void cleanup_messages(void) {
    Message *m = messages;
    while (m) {
        Message *tmp = m;
        m = m->next;
        free(tmp->sender);
        free(tmp->subject);
        free(tmp->body);
        Tag *t = tmp->tags;
        while (t) {
            Tag *tt = t;
            t = t->next;
            free(tt->name);
            free(tt);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %2047s", cmd, arg1, arg2, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "MAIL") == 0 && n >= 3) {
            char *body = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_message(arg1, arg2, body);
        } else if (strcmp(cmd, "TAG") == 0 && n >= 3) {
            add_tag(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0 && n >= 3) {
            remove_tag(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_message(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    cleanup_messages();
    return 0;
}