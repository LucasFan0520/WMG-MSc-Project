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

Message *head = NULL, *tail = NULL;

Message *find_by_subject(const char *subject) {
    Message *m = head;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_message(const char *sender, const char *subject, const char *body) {
    Message *m = malloc(sizeof(Message));
    m->sender = malloc(strlen(sender) + 1);
    strcpy(m->sender, sender);
    m->subject = malloc(strlen(subject) + 1);
    strcpy(m->subject, subject);
    m->body = malloc(strlen(body) + 1);
    strcpy(m->body, body);
    m->tags = NULL;
    m->next = NULL;
    if (tail) {
        tail->next = m;
        tail = m;
    } else {
        head = tail = m;
    }
}

void tag_subject(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = malloc(sizeof(Tag));
    t->tag = malloc(strlen(tag) + 1);
    strcpy(t->tag, tag);
    t->next = m->tags;
    m->tags = t;
}

void untag_subject(const char *subject, const char *tag) {
    Message *m = find_by_subject(subject);
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

void delete_message(const char *subject) {
    Message *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->sender);
            free(curr->subject);
            free(curr->body);
            Tag *t = curr->tags;
            while (t) {
                Tag *next = t->next;
                free(t->tag);
                free(t);
                t = next;
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
    while (m) {
        Tag *t = m->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = strtok(NULL, " ");
            char *subject = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (sender && subject && body) add_message(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = strtok(NULL, " ");
            char *tag = strtok(NULL, " ");
            if (subject && tag) tag_subject(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = strtok(NULL, " ");
            char *tag = strtok(NULL, " ");
            if (subject && tag) untag_subject(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = strtok(NULL, " ");
            if (subject) delete_message(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free(line);
    free_all();
    return 0;
}