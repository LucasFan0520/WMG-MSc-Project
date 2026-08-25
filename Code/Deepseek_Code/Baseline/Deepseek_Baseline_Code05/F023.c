/* F023.c */
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

Message *msgs = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = msgs;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *sender, const char *subject, const char *body) {
    if (find_by_subject(subject)) return; // avoid duplicate subjects? spec says first matching, so we assume unique subjects? Not specified, but we'll allow multiple? They said "first matching subject" for TAG/UNTAG/DELETE, so we need to find first, but we can still have duplicates. We'll just insert at head.
    Message *m = malloc(sizeof(Message));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = msgs;
    msgs = m;
}

void add_tag(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags;
    while (t) { if (strcmp(t->name, tag) == 0) return; t = t->next; }
    Tag *nt = malloc(sizeof(Tag));
    nt->name = strdup(tag);
    nt->next = m->tags;
    m->tags = nt;
}

void remove_tag(const char *subject, const char *tag) {
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
    Message *m = msgs, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else msgs = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            Tag *t = m->tags;
            while (t) { Tag *tmp = t; t = t->next; free(tmp->name); free(tmp); }
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_tag(const char *tag) {
    Message *m = msgs;
    while (m) {
        Tag *t = m->tags;
        int found = 0;
        while (t) {
            if (strcmp(t->name, tag) == 0) { found = 1; break; }
            t = t->next;
        }
        if (found) printf("%s\n", m->subject);
        m = m->next;
    }
}

void report() {
    Message *m = msgs;
    while (m) {
        printf("%s %s ", m->sender, m->subject);
        for (char *c = m->body; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar(' ');
        Tag *t = m->tags;
        while (t) {
            printf("%s", t->name);
            if (t->next) putchar(' ');
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], sender[256], subject[256], tag[256], body[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(line, "%*s %255s %255s", sender, subject) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(sender) + 1 + strlen(subject) + 1;
            if (*rest == ' ') rest++;
            add_message(sender, subject, rest);
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2)
                add_tag(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2)
                remove_tag(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", subject) == 1) delete_message(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(line, "%*s %255s", tag) == 1) search_tag(tag);
        }
    }
    return 0;
}