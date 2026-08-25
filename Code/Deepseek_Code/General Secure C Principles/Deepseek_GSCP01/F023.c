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
    while (t && strcmp(t->tag, tag) != 0) t = t->next;
    if (t) return;
    Tag *nt = malloc(sizeof(Tag));
    nt->tag = strdup(tag);
    nt->next = m->tags;
    m->tags = nt;
}

void untag_message(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags, *prev = NULL;
    while (t && strcmp(t->tag, tag) != 0) {
        prev = t;
        t = t->next;
    }
    if (!t) return;
    if (prev) prev->next = t->next;
    else m->tags = t->next;
    free(t->tag);
    free(t);
}

void delete_message_by_subject(const char *subject) {
    Message *m = messages, *prev = NULL;
    while (m && strcmp(m->subject, subject) != 0) {
        prev = m;
        m = m->next;
    }
    if (!m) return;
    if (prev) prev->next = m->next;
    else messages = m->next;
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

void report_all(void) {
    Message *m = messages;
    while (m) {
        printf("%s %s ", m->sender, m->subject);
        for (char *c = m->body; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        Tag *t = m->tags;
        while (t) {
            printf(" %s", t->tag);
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    Message *m = messages;
    while (m) {
        Message *next = m->next;
        free(m->sender);
        free(m->subject);
        free(m->body);
        Tag *t = m->tags;
        while (t) {
            Tag *nextt = t->next;
            free(t->tag);
            free(t);
            t = nextt;
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
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256];
            char *p = line + 5;
            if (sscanf(p, "%255s %255s", sender, subject) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_message(sender, subject, p);
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(line + 4, "%255s %255s", subject, tag) == 2)
                tag_message(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(line + 6, "%255s %255s", subject, tag) == 2)
                untag_message(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_message_by_subject(p);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            search_tag(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}