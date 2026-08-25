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
    m->sender = malloc(strlen(sender) + 1);
    strcpy(m->sender, sender);
    m->subject = malloc(strlen(subject) + 1);
    strcpy(m->subject, subject);
    m->body = malloc(strlen(body) + 1);
    strcpy(m->body, body);
    m->tags = NULL;
    m->next = head;
    head = m;
}

void tag_subject(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return;
        t = t->next;
    }
    Tag *new = malloc(sizeof(Tag));
    new->tag = malloc(strlen(tag) + 1);
    strcpy(new->tag, tag);
    new->next = m->tags;
    m->tags = new;
}

void untag_subject(const char *subject, const char *tag) {
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

void delete_subject(const char *subject) {
    Message *prev = NULL;
    Message *m = head;
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

void report_messages(void) {
    Message *m = head;
    while (m) {
        printf("%s %s ", m->sender, m->subject);
        for (char *c = m->body; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        printf(" tags:");
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
    Message *m = head;
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_message(sender, subject, p);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            tag_subject(subject, p);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            untag_subject(subject, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_subject(p);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            search_tag(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free_all();
    return 0;
}