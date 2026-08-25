// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tag {
    char *text;
    struct tag *next;
} Tag;

typedef struct msg {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct msg *next;
} Msg;

Msg *msgs = NULL;

Msg *find_msg_by_subject(const char *subject) {
    Msg *m = msgs;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_tag_to_msg(Msg *m, const char *tag) {
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->text, tag) == 0) return;
        t = t->next;
    }
    Tag *new = malloc(sizeof(Tag));
    new->text = strdup(tag);
    new->next = m->tags;
    m->tags = new;
}

void remove_tag_from_msg(Msg *m, const char *tag) {
    Tag *t = m->tags, *prev = NULL;
    while (t) {
        if (strcmp(t->text, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->text);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void mail_message(const char *sender, const char *subject, const char *body) {
    if (find_msg_by_subject(subject)) return;
    Msg *m = malloc(sizeof(Msg));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = msgs;
    msgs = m;
}

void tag_subject(const char *subject, const char *tag) {
    Msg *m = find_msg_by_subject(subject);
    if (m) add_tag_to_msg(m, tag);
}

void untag_subject(const char *subject, const char *tag) {
    Msg *m = find_msg_by_subject(subject);
    if (m) remove_tag_from_msg(m, tag);
}

void delete_subject(const char *subject) {
    Msg *m = msgs, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else msgs = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            Tag *t = m->tags;
            while (t) { Tag *tmp = t; t = t->next; free(tmp->text); free(tmp); }
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_tag(const char *tag) {
    Msg *m = msgs;
    while (m) {
        Tag *t = m->tags;
        while (t) {
            if (strcmp(t->text, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report_messages(void) {
    Msg *m = msgs;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        Tag *t = m->tags;
        while (t) { printf(" %s", t->text); t = t->next; }
        putchar('\n');
        m = m->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(line, "%*s %255s %255s %[^\n]", arg1, arg2, arg3) == 3)
                mail_message(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                tag_subject(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                untag_subject(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_subject(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    return 0;
}