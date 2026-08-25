// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag {
    char *text;
    struct Tag *next;
} Tag;

typedef struct Message {
    char *sender;
    char *subject;
    char *body;
    Tag *tag_head;
    struct Message *next;
} Message;

Message *m_head = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = m_head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

Tag *find_tag(Message *m, const char *tag) {
    Tag *t = m->tag_head;
    while (t) {
        if (strcmp(t->text, tag) == 0) return t;
        t = t->next;
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
    m->tag_head = NULL;
    m->next = m_head;
    m_head = m;
}

void tag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    if (find_tag(m, tag)) return;
    Tag *t = malloc(sizeof(Tag));
    if (!t) return;
    t->text = strdup(tag);
    t->next = m->tag_head;
    m->tag_head = t;
}

void untag_message(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL;
    Tag *t = m->tag_head;
    while (t) {
        if (strcmp(t->text, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tag_head = t->next;
            free(t->text);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void delete_message(const char *subject) {
    Message *prev = NULL;
    Message *m = m_head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else m_head = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            Tag *t = m->tag_head;
            while (t) {
                Tag *next = t->next;
                free(t->text);
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
    Message *m = m_head;
    while (m) {
        Tag *t = m->tag_head;
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

void report_all(void) {
    Message *m = m_head;
    while (m) {
        printf("%s %s ", m->sender, m->subject);
        for (char *p = m->body; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        Tag *t = m->tag_head;
        while (t) {
            printf("  %s\n", t->text);
            t = t->next;
        }
        m = m->next;
    }
}

void free_all(void) {
    while (m_head) {
        Message *next = m_head->next;
        free(m_head->sender);
        free(m_head->subject);
        free(m_head->body);
        Tag *t = m_head->tag_head;
        while (t) {
            Tag *nextt = t->next;
            free(t->text);
            free(t);
            t = nextt;
        }
        free(m_head);
        m_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256], body[1024];
            if (sscanf(line, "%*s %255s %255s %1023[^\n]", sender, subject, body) == 3) {
                add_message(sender, subject, body);
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2) {
                tag_message(subject, tag);
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(line, "%*s %255s %255s", subject, tag) == 2) {
                untag_message(subject, tag);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subject[256];
            if (sscanf(line, "%*s %255s", subject) == 1) {
                delete_message(subject);
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256];
            if (sscanf(line, "%*s %255s", tag) == 1) {
                search_tag(tag);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}