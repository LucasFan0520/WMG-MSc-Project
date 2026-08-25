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

Message *head = NULL;

Message *find_message_by_subject(const char *subject) {
    Message *m = head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

Tag *find_tag(Message *m, const char *tag) {
    Tag *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return t;
        t = t->next;
    }
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
    m->next = head;
    head = m;
}

void tag_add(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    if (find_tag(m, tag)) return;
    Tag *t = malloc(sizeof(Tag));
    if (!t) return;
    t->tag = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void tag_remove(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL, *curr = m->tags;
    while (curr) {
        if (strcmp(curr->tag, tag) == 0) {
            if (prev) prev->next = curr->next;
            else m->tags = curr->next;
            free(curr->tag);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void message_delete(const char *subject) {
    Message *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->sender);
            free(curr->subject);
            free(curr->body);
            Tag *t = curr->tags;
            while (t) {
                Tag *tmp = t;
                t = t->next;
                free(tmp->tag);
                free(tmp);
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void search_tag(const char *tag) {
    Message *m = head;
    int first = 1;
    while (m) {
        if (find_tag(m, tag)) {
            if (!first) printf(" ");
            printf("%s", m->subject);
            first = 0;
        }
        m = m->next;
    }
    printf("\n");
}

void report_messages(void) {
    Message *m = head;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        Tag *t = m->tags;
        while (t) {
            printf(" %s", t->tag);
            t = t->next;
        }
        printf("\n");
        m = m->next;
    }
}

void free_all(void) {
    Message *m = head;
    while (m) {
        Message *tmp = m;
        m = m->next;
        free(tmp->sender);
        free(tmp->subject);
        free(tmp->body);
        Tag *t = tmp->tags;
        while (t) {
            Tag *tmp_t = t;
            t = t->next;
            free(tmp_t->tag);
            free(tmp_t);
        }
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256];
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", sender, subject) == 2) {
                char *body = p;
                while (*body && *body != ' ') body++;
                while (*body && *body == ' ') body++;
                while (*body && *body != ' ') body++;
                while (*body && *body == ' ') body++;
                if (*body) {
                    mail_add(sender, subject, body);
                }
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(line + 4, "%255s %255s", subject, tag) == 2) {
                tag_add(subject, tag);
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subject[256], tag[256];
            if (sscanf(line + 6, "%255s %255s", subject, tag) == 2) {
                tag_remove(subject, tag);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            message_delete(p);
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