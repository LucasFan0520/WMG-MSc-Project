// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag { char *tag; struct Tag *next; } Tag;
typedef struct Message { char *sender; char *subject; char *body; Tag *tags; struct Message *next; } Message;
Message *messages = NULL;

Message *find_message_by_subject(const char *subject) {
    for (Message *m = messages; m; m = m->next)
        if (strcmp(m->subject, subject) == 0) return m;
    return NULL;
}

void mail_add(const char *sender, const char *subject, const char *body) {
    if (find_message_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

void tag_subject(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    for (Tag *t = m->tags; t; t = t->next)
        if (strcmp(t->tag, tag) == 0) return;
    Tag *t = malloc(sizeof(Tag));
    if (!t) return;
    t->tag = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void untag_subject(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL;
    for (Tag *t = m->tags; t; prev = t, t = t->next) {
        if (strcmp(t->tag, tag) == 0) {
            if (prev) prev->next = t->next; else m->tags = t->next;
            free(t->tag); free(t);
            return;
        }
    }
}

void delete_subject(const char *subject) {
    Message *prev = NULL;
    for (Message *m = messages; m; prev = m, m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next; else messages = m->next;
            free(m->sender); free(m->subject); free(m->body);
            while (m->tags) { Tag *t = m->tags; m->tags = t->next; free(t->tag); free(t); }
            free(m);
            return;
        }
    }
}

void search_tag(const char *tag) {
    for (Message *m = messages; m; m = m->next) {
        for (Tag *t = m->tags; t; t = t->next)
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
    }
}

void report_messages(void) {
    for (Message *m = messages; m; m = m->next) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        for (Tag *t = m->tags; t; t = t->next) printf(" %s", t->tag);
        putchar('\n');
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256], body[4096];
            if (sscanf(rest, "%255s %255s", sender, subject) == 2) {
                char *b = rest + strlen(sender) + 1 + strlen(subject);
                while (*b == ' ') b++;
                strcpy(body, b);
                mail_add(sender, subject, body);
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(rest, "%255s %255s", subject, tag) == 2) tag_subject(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(rest, "%255s %255s", subject, tag) == 2) untag_subject(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256];
            if (sscanf(rest, "%255s", subject) == 1) delete_subject(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256];
            if (sscanf(rest, "%255s", tag) == 1) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    while (messages) { Message *m = messages; messages = m->next; free(m->sender); free(m->subject); free(m->body); while (m->tags) { Tag *t = m->tags; m->tags = t->next; free(t->tag); free(t); } free(m); }
    return 0;
}