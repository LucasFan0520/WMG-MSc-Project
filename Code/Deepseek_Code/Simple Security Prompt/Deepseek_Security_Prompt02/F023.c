/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

Message *find_message_by_subject(const char *subject) {
    Message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void free_tags(Tag *t) {
    while (t) {
        Tag *tmp = t;
        t = t->next;
        free(tmp->name);
        free(tmp);
    }
}

void free_message(Message *m) {
    free(m->sender);
    free(m->subject);
    free(m->body);
    free_tags(m->tags);
    free(m);
}

void mail_add(const char *sender, const char *subject, const char *body) {
    if (find_message_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
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
    Tag *t = malloc(sizeof(Tag));
    t->name = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void untag_subject(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL, *curr = m->tags;
    while (curr) {
        if (strcmp(curr->name, tag) == 0) {
            if (prev) prev->next = curr->next;
            else m->tags = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_subject(const char *subject) {
    Message *prev = NULL, *curr = messages;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            if (prev) prev->next = curr->next;
            else messages = curr->next;
            free_message(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void search_tag(const char *tag) {
    Message *m = messages;
    int found = 0;
    while (m) {
        Tag *t = m->tags;
        while (t) {
            if (strcmp(t->name, tag) == 0) {
                printf("%s\n", m->subject);
                found = 1;
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
    if (!found) printf("NOT_FOUND\n");
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

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char sender[256], subject[256], tag[256], body[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "MAIL") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", sender, subject) != 2) continue;
            p += strlen(sender) + strlen(subject) + 2;
            while (*p == ' ') p++;
            mail_add(sender, subject, p);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", subject, tag) == 2) tag_subject(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", subject, tag) == 2) untag_subject(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", subject) == 1) delete_subject(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", tag) == 1) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }

    while (messages) {
        Message *tmp = messages;
        messages = messages->next;
        free_message(tmp);
    }
    return 0;
}