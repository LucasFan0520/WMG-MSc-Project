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
Message *tail = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *sender, const char *subject, const char *body) {
    if (find_by_subject(subject)) return; // first matching? we allow only one per subject? The spec says "first matching subject" so we assume subject unique for deletion/tag.
    Message *m = malloc(sizeof(Message));
    m->sender = malloc(strlen(sender)+1);
    strcpy(m->sender, sender);
    m->subject = malloc(strlen(subject)+1);
    strcpy(m->subject, subject);
    m->body = malloc(strlen(body)+1);
    strcpy(m->body, body);
    m->tags = NULL;
    m->next = NULL;
    if (tail) tail->next = m;
    else head = m;
    tail = m;
}

void add_tag_to_subject(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = malloc(sizeof(Tag));
    t->tag = malloc(strlen(tag)+1);
    strcpy(t->tag, tag);
    t->next = m->tags;
    m->tags = t;
}

void remove_tag_from_subject(const char *subject, const char *tag) {
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

void delete_message(const char *subject) {
    Message *prev = NULL;
    Message *m = head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            if (m == tail) tail = prev;
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
        printf("%s %s %s", m->sender, m->subject, m->body);
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
            Tag *tnext = t->next;
            free(t->tag);
            free(t);
            t = tnext;
        }
        free(m);
        m = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = strtok(rest, " ");
            if (!sender) continue;
            char *subject = strtok(NULL, " ");
            if (!subject) continue;
            char *body = strtok(NULL, "");
            if (!body) body = "";
            add_message(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = strtok(rest, " ");
            if (!subject) continue;
            char *tag = strtok(NULL, " ");
            if (tag) add_tag_to_subject(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = strtok(rest, " ");
            if (!subject) continue;
            char *tag = strtok(NULL, " ");
            if (tag) remove_tag_from_subject(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = strtok(rest, " ");
            if (subject) delete_message(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = strtok(rest, " ");
            if (tag) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free_all();
    return 0;
}