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

Message *find_message_by_subject(const char *subject) {
    Message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *sender, const char *subject, const char *body) {
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

void tag_message(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->name, tag) == 0) return;
        t = t->next;
    }
    Tag *new = malloc(sizeof(Tag));
    if (!new) return;
    new->name = strdup(tag);
    new->next = m->tags;
    m->tags = new;
}

void untag_message(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL, *cur = m->tags;
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
    Message *prev = NULL, *cur = messages;
    while (cur) {
        if (strcmp(cur->subject, subject) == 0) {
            if (prev) prev->next = cur->next;
            else messages = cur->next;
            Tag *t = cur->tags;
            while (t) {
                Tag *next = t->next;
                free(t->name);
                free(t);
                t = next;
            }
            free(cur->sender);
            free(cur->subject);
            free(cur->body);
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
            if (strcmp(t->name, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report_messages(void) {
    Message *m = messages;
    while (m) {
        printf("From: %s\nSubject: %s\n", m->sender, m->subject);
        char *s = m->body;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        Tag *t = m->tags;
        if (t) {
            printf("Tags: ");
            while (t) {
                printf("%s", t->name);
                if (t->next) printf(", ");
                t = t->next;
            }
            putchar('\n');
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    Message *m = messages;
    while (m) {
        Message *next = m->next;
        Tag *t = m->tags;
        while (t) {
            Tag *tnext = t->next;
            free(t->name);
            free(t);
            t = tnext;
        }
        free(m->sender);
        free(m->subject);
        free(m->body);
        free(m);
        m = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *sender = p;
            char *space = strchr(sender, ' ');
            if (!space) continue;
            *space = '\0';
            char *subject = space + 1;
            while (*subject == ' ') subject++;
            space = strchr(subject, ' ');
            if (!space) continue;
            *space = '\0';
            char *body = space + 1;
            while (*body == ' ') body++;
            add_message(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *subject = p;
            char *space = strchr(subject, ' ');
            if (!space) continue;
            *space = '\0';
            char *tag = space + 1;
            while (*tag == ' ') tag++;
            tag_message(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *subject = p;
            char *space = strchr(subject, ' ');
            if (!space) continue;
            *space = '\0';
            char *tag = space + 1;
            while (*tag == ' ') tag++;
            untag_message(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *subject = p;
            char *space = strchr(subject, ' ');
            if (space) *space = '\0';
            delete_message(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag = p;
            char *space = strchr(tag, ' ');
            if (space) *space = '\0';
            search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free_all();
    return 0;
}